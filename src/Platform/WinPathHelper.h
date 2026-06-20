#pragma once
#include <string>

namespace ExplorerX::Platform {

class WinPathHelper {
public:
    static std::wstring NormalizeAndResolvePath(const std::string& utf8Path);
    static std::wstring Utf8ToWide(const std::string& utf8);
    static std::string WideToUtf8(const std::wstring& wide);
};

} // namespace ExplorerX::Platform
