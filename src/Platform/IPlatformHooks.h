#pragma once

namespace ExplorerX::Platform {

class IPlatformHooks {
public:
    virtual ~IPlatformHooks() = default;

    // Enables OS-level blur (Mica/Acrylic) and optionally forces the dark title bar on Windows 11.
    static void EnableWindowBlur(void* hwnd, bool enableDarkMica);
};

} // namespace ExplorerX::Platform
