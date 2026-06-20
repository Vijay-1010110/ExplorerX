#include "WinPathHelper.h"
#include <windows.h>
#include <vector>

namespace ExplorerX::Platform {

std::wstring WinPathHelper::Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return {};
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), nullptr, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), result.data(), size_needed);
    return result;
}

std::string WinPathHelper::WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return {};
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), result.data(), size_needed, nullptr, nullptr);
    return result;
}

std::wstring WinPathHelper::NormalizeAndResolvePath(const std::string& utf8Path) {
    std::wstring wPath = Utf8ToWide(utf8Path);
    if (wPath.empty()) return {};

    HANDLE hFile = CreateFileW(wPath.c_str(),
                               0, 
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_FLAG_BACKUP_SEMANTICS,
                               NULL);

    if (hFile != INVALID_HANDLE_VALUE) {
        std::vector<wchar_t> buffer(MAX_PATH);
        DWORD result = GetFinalPathNameByHandleW(hFile, buffer.data(), (DWORD)buffer.size(), FILE_NAME_NORMALIZED);
        if (result > buffer.size()) {
            buffer.resize(result);
            result = GetFinalPathNameByHandleW(hFile, buffer.data(), (DWORD)buffer.size(), FILE_NAME_NORMALIZED);
        }
        CloseHandle(hFile);

        if (result > 0 && result < buffer.size()) {
            return std::wstring(buffer.data(), result);
        }
    }

    std::vector<wchar_t> fullPath(32767);
    DWORD len = GetFullPathNameW(wPath.c_str(), (DWORD)fullPath.size(), fullPath.data(), nullptr);
    if (len > 0 && len < fullPath.size()) {
        std::wstring resolved(fullPath.data(), len);
        if (resolved.length() >= MAX_PATH && resolved.compare(0, 4, L"\\\\?\\") != 0) {
            if (resolved.compare(0, 2, L"\\\\") == 0) {
                resolved = L"\\\\?\\UNC\\" + resolved.substr(2);
            } else {
                resolved = L"\\\\?\\" + resolved;
            }
        }
        return resolved;
    }

    return wPath;
}

} // namespace ExplorerX::Platform
