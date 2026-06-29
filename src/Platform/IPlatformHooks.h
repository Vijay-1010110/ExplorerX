#pragma once
#include <QtGlobal>

namespace ExplorerX::Platform {

class IPlatformHooks {
public:
    virtual ~IPlatformHooks() = default;

    // Enables OS-level blur (Mica/Acrylic) and optionally forces the dark title bar on Windows 11.
    static void EnableWindowBlur(void* hwnd, bool enableDarkMica);

    // Provide a hook for WM_NCHITTEST for frameless window dragging.
    static bool HandleNCHitTest(void* msg, qintptr* result, int captionHeight);
};

} // namespace ExplorerX::Platform
