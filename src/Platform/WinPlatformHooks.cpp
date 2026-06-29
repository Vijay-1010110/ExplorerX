#include "IPlatformHooks.h"

#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <shlobj.h>
#include <wrl/client.h>

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

void IPlatformHooks::ShowNativeContextMenu(void* hwnd, const std::string& filePath, int x, int y) {
#ifdef _WIN32
    if (!hwnd || filePath.empty()) return;

    HWND hWnd = static_cast<HWND>(hwnd);

    // Initialize COM if not already initialized on this thread
    HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool uninitCom = SUCCEEDED(hrInit);

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &filePath[0], (int)filePath.size(), NULL, 0);
    std::wstring wFilePath(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &filePath[0], (int)filePath.size(), &wFilePath[0], size_needed);

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(wFilePath.c_str(), nullptr, &pidl, 0, nullptr))) {
        
        Microsoft::WRL::ComPtr<IShellFolder> pDesktop;
        if (SUCCEEDED(SHGetDesktopFolder(&pDesktop))) {
            
            PCUITEMID_CHILD pidlChild;
            Microsoft::WRL::ComPtr<IShellFolder> pParentFolder;
            if (SUCCEEDED(SHBindToParent(pidl, IID_PPV_ARGS(&pParentFolder), &pidlChild))) {
                
                Microsoft::WRL::ComPtr<IContextMenu> pContextMenu;
                if (SUCCEEDED(pParentFolder->GetUIObjectOf(hWnd, 1, (LPCITEMIDLIST*)&pidlChild, IID_IContextMenu, nullptr, reinterpret_cast<void**>(pContextMenu.GetAddressOf())))) {
                    
                    HMENU hMenu = CreatePopupMenu();
                    if (hMenu) {
                        if (SUCCEEDED(pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {
                            int command = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, x, y, hWnd, nullptr);
                            if (command > 0) {
                                CMINVOKECOMMANDINFO cmi = {0};
                                cmi.cbSize = sizeof(cmi);
                                cmi.fMask = 0;
                                cmi.hwnd = hWnd;
                                cmi.lpVerb = MAKEINTRESOURCEA(command - 1);
                                cmi.nShow = SW_SHOWNORMAL;
                                
                                pContextMenu->InvokeCommand(&cmi);
                            }
                        }
                        DestroyMenu(hMenu);
                    }
                }
            }
        }
        CoTaskMemFree(pidl);
    }

    if (uninitCom) {
        CoUninitialize();
    }
#endif
}

} // namespace ExplorerX::Platform
