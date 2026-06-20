#pragma once

#include "IFileSystemWatcher.h"
#include <windows.h>
#include <thread>
#include <atomic>
#include <string>

namespace ExplorerX::Platform {

class WinFileSystemWatcher : public Domain::IFileSystemWatcher {
public:
    WinFileSystemWatcher();
    ~WinFileSystemWatcher() override;

    void StartWatching(const Domain::Path& directoryPath, std::function<void(const Domain::FileSystemEvent&)> callback) override;
    void StopWatching() override;

private:
    void WatchThread();

    std::atomic<bool> m_running;
    std::thread m_watchThread;
    std::function<void(const Domain::FileSystemEvent&)> m_callback;
    Domain::Path m_watchedPath;

    HANDLE m_hDir;
    HANDLE m_hCompletionPort;

    static constexpr DWORD BUFFER_SIZE = 64 * 1024;
    alignas(DWORD) uint8_t m_buffer[BUFFER_SIZE];
    OVERLAPPED m_overlapped;
};

} // namespace ExplorerX::Platform
