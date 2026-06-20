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

std::future<Domain::Expected<Domain::FileItem>> WinFileSystemProvider::GetMetadata(const Domain::Path& path) {
    return std::async(std::launch::async, [pathString = path.ToString(), pathObj = path]() -> Domain::Expected<Domain::FileItem> {
        std::wstring wPath = Utf8ToWide(pathString);
        if (wPath.empty()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty path", 0});
        }

        WIN32_FILE_ATTRIBUTE_DATA attrData;
        if (!GetFileAttributesExW(wPath.c_str(), GetFileExInfoStandard, &attrData)) {
            DWORD err = GetLastError();
            return Domain::MakeUnexpected(Domain::Error{MapWin32Error(err), "GetFileAttributesExW failed", (int)err});
        }

        Domain::FileItem item;
        item.ItemPath = pathObj;
        
        size_t lastSlash = pathString.find_last_of("\\/");
        if (lastSlash != std::string::npos && lastSlash + 1 < pathString.length()) {
            item.Name = pathString.substr(lastSlash + 1);
        } else {
            item.Name = pathString;
        }

        ULARGE_INTEGER sz;
        sz.LowPart = attrData.nFileSizeLow;
        sz.HighPart = attrData.nFileSizeHigh;
        item.Size = sz.QuadPart;

        item.IsDirectory = (attrData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        item.IsHidden = (attrData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
        item.IsSystem = (attrData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;

        item.DateCreated = FileTimeToTimePoint(attrData.ftCreationTime);
        item.DateModified = FileTimeToTimePoint(attrData.ftLastWriteTime);
        item.DateAccessed = FileTimeToTimePoint(attrData.ftLastAccessTime);

        return item;
    });
}


struct CopyContextData {
    const Domain::ProgressContext* ctx;
    std::string fileName;
};

static DWORD CALLBACK CopyProgressCallbackWithData(
    LARGE_INTEGER TotalFileSize,
    LARGE_INTEGER TotalBytesTransferred,
    LARGE_INTEGER StreamSize,
    LARGE_INTEGER StreamBytesTransferred,
    DWORD dwStreamNumber,
    DWORD dwCallbackReason,
    HANDLE hSourceFile,
    HANDLE hDestinationFile,
    LPVOID lpData
) {
    auto* data = static_cast<CopyContextData*>(lpData);
    if (data && data->ctx) {
        if (data->ctx->IsCancelled && data->ctx->IsCancelled->load()) {
            return PROGRESS_CANCEL;
        }
        if (data->ctx->OnProgress) {
            data->ctx->OnProgress(TotalBytesTransferred.QuadPart, TotalFileSize.QuadPart, data->fileName);
        }
    }
    return PROGRESS_CONTINUE;
}

std::future<Domain::Expected<void>> WinFileSystemProvider::Copy(const Domain::CopyRequest& req, const Domain::ProgressContext& progress) {
    return std::async(std::launch::async, [req, progress]() -> Domain::Expected<void> {
        std::wstring src = Utf8ToWide(req.Source.ToString());
        std::wstring dst = Utf8ToWide(req.Destination.ToString());
        
        if (src.empty() || dst.empty()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty path", 0});
        }

        BOOL cancelFlag = FALSE;
        CopyContextData data = { &progress, req.Source.ToString() };
        
        DWORD copyFlags = req.Overwrite ? 0 : COPY_FILE_FAIL_IF_EXISTS;

        if (!CopyFileExW(src.c_str(), dst.c_str(), CopyProgressCallbackWithData, &data, &cancelFlag, copyFlags)) {
            DWORD err = GetLastError();
            if (err == ERROR_REQUEST_ABORTED) {
                return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::OperationCancelled, "Copy cancelled", (int)err});
            }
            return Domain::MakeUnexpected(Domain::Error{MapWin32Error(err), "CopyFileExW failed", (int)err});
        }
        
        return {};
    });
}

std::future<Domain::Expected<void>> WinFileSystemProvider::Move(const Domain::MoveRequest& req, const Domain::ProgressContext& progress) {
    return std::async(std::launch::async, [req, progress]() -> Domain::Expected<void> {
        std::wstring src = Utf8ToWide(req.Source.ToString());
        std::wstring dst = Utf8ToWide(req.Destination.ToString());
        
        if (src.empty() || dst.empty()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty path", 0});
        }

        CopyContextData data = { &progress, req.Source.ToString() };
        
        if (!MoveFileWithProgressW(src.c_str(), dst.c_str(), CopyProgressCallbackWithData, &data, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
            DWORD err = GetLastError();
            if (err == ERROR_REQUEST_ABORTED) {
                return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::OperationCancelled, "Move cancelled", (int)err});
            }
            return Domain::MakeUnexpected(Domain::Error{MapWin32Error(err), "MoveFileWithProgressW failed", (int)err});
        }
        
        return {};
    });
}

#include <shellapi.h>

std::future<Domain::Expected<void>> WinFileSystemProvider::Delete(const Domain::DeleteRequest& req, const Domain::ProgressContext& progress) {
    return std::async(std::launch::async, [req, progress]() -> Domain::Expected<void> {
        std::wstring target = Utf8ToWide(req.Target.ToString());
        if (target.empty()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty path", 0});
        }

        if (progress.IsCancelled && progress.IsCancelled->load()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::OperationCancelled, "Delete cancelled", 0});
        }

        if (req.Permanent) {
            DWORD attr = GetFileAttributesW(target.c_str());
            if (attr == INVALID_FILE_ATTRIBUTES) {
                return Domain::MakeUnexpected(Domain::Error{MapWin32Error(GetLastError()), "File not found", (int)GetLastError()});
            }
            if (attr & FILE_ATTRIBUTE_DIRECTORY) {
                if (!RemoveDirectoryW(target.c_str())) {
                    return Domain::MakeUnexpected(Domain::Error{MapWin32Error(GetLastError()), "RemoveDirectoryW failed", (int)GetLastError()});
                }
            } else {
                if (!DeleteFileW(target.c_str())) {
                    return Domain::MakeUnexpected(Domain::Error{MapWin32Error(GetLastError()), "DeleteFileW failed", (int)GetLastError()});
                }
            }
        } else {
            // Send to recycle bin
            std::wstring doubleNullPath = target;
            doubleNullPath.push_back(L'\0');

            SHFILEOPSTRUCTW fileOp = {};
            fileOp.hwnd = NULL;
            fileOp.wFunc = FO_DELETE;
            fileOp.pFrom = doubleNullPath.c_str();
            fileOp.pTo = NULL;
            fileOp.fFlags = FOF_ALLOWUNDO | FOF_NO_UI | FOF_SILENT | FOF_NOCONFIRMATION;

            int result = SHFileOperationW(&fileOp);
            if (result != 0 || fileOp.fAnyOperationsAborted) {
                return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "SHFileOperationW failed to recycle", result});
            }
        }

        if (progress.OnProgress) {
            progress.OnProgress(1, 1, req.Target.ToString());
        }

        return {};
    });
}

} // namespace ExplorerX::Platform
