#include "WinFileSystemProvider.h"
#include <windows.h>
#include <string>
#include <vector>

namespace ExplorerX::Platform {

static std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return {};
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), nullptr, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), result.data(), size_needed);
    return result;
}

static std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), result.data(), size_needed, nullptr, nullptr);
    return result;
}

static std::chrono::system_clock::time_point FileTimeToTimePoint(const FILETIME& ft) {
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;
    long long timeSince1970 = ull.QuadPart - 116444736000000000ULL;
    auto d = std::chrono::duration<long long, std::ratio<1, 10000000>>(timeSince1970);
    return std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(d));
}

static Domain::ErrorCode MapWin32Error(DWORD err) {
    switch(err) {
        case ERROR_SUCCESS: return Domain::ErrorCode::Success;
        case ERROR_ACCESS_DENIED: return Domain::ErrorCode::AccessDenied;
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND: return Domain::ErrorCode::PathNotFound;
        case ERROR_SHARING_VIOLATION: return Domain::ErrorCode::SharingViolation;
        default: return Domain::ErrorCode::Unknown;
    }
}

std::future<Domain::Expected<Domain::ListingResult>> WinFileSystemProvider::Enumerate(const Domain::Path& path) {
    return std::async(std::launch::async, [pathString = path.ToString()]() -> Domain::Expected<Domain::ListingResult> {
        Domain::ListingResult result;
        
        std::wstring wPath = Utf8ToWide(pathString);
        if (wPath.empty()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty path", 0});
        }
        
        if (wPath.back() != L'\\' && wPath.back() != L'/') {
            wPath += L'\\';
        }
        wPath += L'*';
        
        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileExW(wPath.c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
        
        if (hFind == INVALID_HANDLE_VALUE) {
            DWORD err = GetLastError();
            return Domain::MakeUnexpected(Domain::Error{MapWin32Error(err), "FindFirstFileExW failed", (int)err});
        }
        
        do {
            std::wstring fileName = findData.cFileName;
            if (fileName == L"." || fileName == L"..") {
                continue;
            }
            
            Domain::FileItem item;
            item.Name = WideToUtf8(fileName);
            
            std::string dirPath = pathString;
            if (!dirPath.empty() && dirPath.back() != '\\' && dirPath.back() != '/') {
                dirPath += '\\';
            }
            item.ItemPath = Domain::Path(dirPath + item.Name);
            
            ULARGE_INTEGER sz;
            sz.LowPart = findData.nFileSizeLow;
            sz.HighPart = findData.nFileSizeHigh;
            item.Size = sz.QuadPart;
            
            item.IsDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            item.IsHidden = (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
            item.IsSystem = (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;
            
            item.DateCreated = FileTimeToTimePoint(findData.ftCreationTime);
            item.DateModified = FileTimeToTimePoint(findData.ftLastWriteTime);
            item.DateAccessed = FileTimeToTimePoint(findData.ftLastAccessTime);
            
            result.Items.push_back(std::move(item));
        } while (FindNextFileW(hFind, &findData) != 0);
        
        DWORD err = GetLastError();
        FindClose(hFind);
        
        if (err != ERROR_NO_MORE_FILES) {
            return Domain::MakeUnexpected(Domain::Error{MapWin32Error(err), "FindNextFileW error", (int)err});
        }
        
        return result;
    });
}

Domain::Expected<void> WinFileSystemProvider::Copy(const Domain::CopyRequest& req) {
    return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Not implemented yet", 0});
}

Domain::Expected<void> WinFileSystemProvider::Move(const Domain::MoveRequest& req) {
    return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Not implemented yet", 0});
}

Domain::Expected<void> WinFileSystemProvider::Delete(const Domain::DeleteRequest& req) {
    return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Not implemented yet", 0});
}

} // namespace ExplorerX::Platform
