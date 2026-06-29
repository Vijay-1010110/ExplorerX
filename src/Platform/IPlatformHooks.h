#pragma once
#include <QtGlobal>
#include <string>

namespace ExplorerX::Platform {

class IPlatformHooks {
public:
    virtual ~IPlatformHooks() = default;

    // Enables OS-level blur (Mica/Acrylic) and optionally forces the dark title bar on Windows 11.
    static void EnableWindowBlur(void* hwnd, bool enableDarkMica);

    // Provide a hook for WM_NCHITTEST for frameless window dragging.
    static bool HandleNCHitTest(void* msg, qintptr* result, int captionHeight);

    // Summons the classic Windows Shell context menu for a specific file
    static void ShowNativeContextMenu(void* hwnd, const std::string& filePath, int x, int y);
};

} // namespace ExplorerX::Platform
