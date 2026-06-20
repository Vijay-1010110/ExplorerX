#include "WinFileSystemWatcher.h"
#include "WinPathHelper.h"
#include <vector>

namespace ExplorerX::Platform {

WinFileSystemWatcher::WinFileSystemWatcher() 
    : m_running(false), m_hDir(INVALID_HANDLE_VALUE), m_hCompletionPort(NULL) {
    memset(&m_overlapped, 0, sizeof(m_overlapped));
}

WinFileSystemWatcher::~WinFileSystemWatcher() {
    StopWatching();
}

void WinFileSystemWatcher::StartWatching(const Domain::Path& directoryPath, std::function<void(const Domain::FileSystemEvent&)> callback) {
    StopWatching();

    m_watchedPath = directoryPath;
    m_callback = callback;
    m_running = true;

    std::wstring wPath = WinPathHelper::NormalizeAndResolvePath(directoryPath.ToString());
    if (wPath.empty()) return;

    m_hDir = CreateFileW(wPath.c_str(),
                         FILE_LIST_DIRECTORY,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         NULL,
                         OPEN_EXISTING,
                         FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                         NULL);

    if (m_hDir == INVALID_HANDLE_VALUE) {
        return;
    }

    m_hCompletionPort = CreateIoCompletionPort(m_hDir, NULL, (ULONG_PTR)this, 1);
    if (!m_hCompletionPort) {
        CloseHandle(m_hDir);
        m_hDir = INVALID_HANDLE_VALUE;
        return;
    }

    memset(&m_overlapped, 0, sizeof(m_overlapped));

    DWORD bytesReturned = 0;
    ReadDirectoryChangesW(m_hDir, m_buffer, BUFFER_SIZE, TRUE,
                          FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                          FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                          FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                          &bytesReturned, &m_overlapped, NULL);

    m_watchThread = std::thread(&WinFileSystemWatcher::WatchThread, this);
}

void WinFileSystemWatcher::StopWatching() {
    m_running = false;
    
    if (m_hCompletionPort) {
        PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, NULL);
    }
    
    if (m_watchThread.joinable()) {
        m_watchThread.join();
    }

    if (m_hDir != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hDir);
        m_hDir = INVALID_HANDLE_VALUE;
    }

    if (m_hCompletionPort) {
        CloseHandle(m_hCompletionPort);
        m_hCompletionPort = NULL;
    }
}

void WinFileSystemWatcher::WatchThread() {
    DWORD numBytes = 0;
    ULONG_PTR completionKey = 0;
    LPOVERLAPPED overlapped = nullptr;
    std::string oldNameForRename;

    while (m_running) {
        BOOL res = GetQueuedCompletionStatus(m_hCompletionPort, &numBytes, &completionKey, &overlapped, INFINITE);

        if (!m_running || completionKey == 0) {
            break; 
        }

        if (res && overlapped && numBytes > 0) {
            FILE_NOTIFY_INFORMATION* notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer);
            while (true) {
                std::wstring fileName(notify->FileName, notify->FileNameLength / sizeof(WCHAR));
                std::string utf8Name = WinPathHelper::WideToUtf8(fileName);
                
                std::string fullPathStr = m_watchedPath.ToString();
                if (!fullPathStr.empty() && fullPathStr.back() != '\\' && fullPathStr.back() != '/') {
                    fullPathStr += '\\';
                }
                fullPathStr += utf8Name;

                Domain::FileSystemEvent evt;
                evt.TargetPath = Domain::Path(fullPathStr);

                bool invokeCallback = true;
                switch (notify->Action) {
                    case FILE_ACTION_ADDED:
                        evt.Type = Domain::FileSystemEventType::Created;
                        break;
                    case FILE_ACTION_REMOVED:
                        evt.Type = Domain::FileSystemEventType::Deleted;
                        break;
                    case FILE_ACTION_MODIFIED:
                        evt.Type = Domain::FileSystemEventType::Modified;
                        break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        oldNameForRename = utf8Name;
                        invokeCallback = false;
                        break;
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        evt.Type = Domain::FileSystemEventType::Renamed;
                        {
                            std::string oldFullPathStr = m_watchedPath.ToString();
                            if (!oldFullPathStr.empty() && oldFullPathStr.back() != '\\' && oldFullPathStr.back() != '/') {
                                oldFullPathStr += '\\';
                            }
                            oldFullPathStr += oldNameForRename;
                            evt.OldPath = Domain::Path(oldFullPathStr);
                        }
                        break;
                    default:
                        invokeCallback = false;
                        break;
                }

                if (invokeCallback && m_callback) {
                    m_callback(evt);
                }

                if (notify->NextEntryOffset == 0) break;
                notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<uint8_t*>(notify) + notify->NextEntryOffset);
            }

            memset(&m_overlapped, 0, sizeof(m_overlapped));
            ReadDirectoryChangesW(m_hDir, m_buffer, BUFFER_SIZE, TRUE,
                                  FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                                  FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                                  FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                                  &numBytes, &m_overlapped, NULL);
        } else if (!res) {
            // CancelIo or handle closed could trigger this
            break;
        }
    }
}

} // namespace ExplorerX::Platform
