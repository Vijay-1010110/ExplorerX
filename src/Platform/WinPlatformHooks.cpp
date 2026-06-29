#include "IPlatformHooks.h"

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <uxtheme.h>

#pragma comment(lib, "dwmapi.lib")

// Define DWMWA constants if running on an older SDK that doesn't have Windows 11 attributes yet
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

#ifndef DWMSBT_MAINWINDOW
#define DWMSBT_MAINWINDOW 2 // Mica
#endif

#ifndef DWMSBT_TRANSIENTWINDOW
#define DWMSBT_TRANSIENTWINDOW 3 // Acrylic
#endif

#endif // _WIN32

namespace ExplorerX::Platform {

void IPlatformHooks::EnableWindowBlur(void* hwnd, bool enableDarkMica) {
#ifdef _WIN32
    if (!hwnd) return;
    HWND hWnd = static_cast<HWND>(hwnd);

    // 1. Enable Immersive Dark Mode for the title bar if requested
    int trueValue = 1;
    int falseValue = 0;
    if (enableDarkMica) {
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &trueValue, sizeof(trueValue));
    } else {
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &falseValue, sizeof(falseValue));
    }

    // 2. Enable System Backdrop (Mica)
    int backdropType = DWMSBT_MAINWINDOW;
    DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

    // 3. Extend frame into client area to maintain drop shadows
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hWnd, &margins);
#endif
}

bool IPlatformHooks::HandleNCHitTest(void* msg, qintptr* result, int captionHeight) {
#ifdef _WIN32
    MSG* message = static_cast<MSG*>(msg);
    if (message->message == WM_NCHITTEST) {
        POINT pt = { GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam) };
        ScreenToClient(message->hwnd, &pt);
        // If the mouse is in the top 'captionHeight' pixels, treat it as the title bar
        if (pt.y < captionHeight) {
            *result = HTCAPTION;
            return true;
        }
    }
#endif
    return false;
}

} // namespace ExplorerX::Platform
