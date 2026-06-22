#include "WinThumbnailProvider.h"
#include "WinPathHelper.h"
#include <windows.h>
#include <shlobj.h>
#include <wincodec.h>
#include <commoncontrols.h>

#pragma comment(lib, "Windowscodecs.lib")

namespace ExplorerX::Platform {

static Domain::Expected<std::vector<uint8_t>> SaveWICBitmapToPNG(IWICImagingFactory* pFactory, IWICBitmap* pWicBitmap) {
    std::vector<uint8_t> result;
    IStream* pStream = NULL;
    HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
    if (FAILED(hr)) return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "CreateStreamOnHGlobal failed", (int)hr});

    IWICBitmapEncoder* pEncoder = NULL;
    hr = pFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
    if (SUCCEEDED(hr)) {
        hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
        if (SUCCEEDED(hr)) {
            IWICBitmapFrameEncode* pFrame = NULL;
            hr = pEncoder->CreateNewFrame(&pFrame, NULL);
            if (SUCCEEDED(hr)) {
                hr = pFrame->Initialize(NULL);
                if (SUCCEEDED(hr)) {
                    UINT width = 0, height = 0;
                    pWicBitmap->GetSize(&width, &height);
                    pFrame->SetSize(width, height);
                    
                    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
                    pFrame->SetPixelFormat(&format);
                    
                    hr = pFrame->WriteSource(pWicBitmap, NULL);
                    if (SUCCEEDED(hr)) {
                        pFrame->Commit();
                        pEncoder->Commit();

                        STATSTG stat;
                        pStream->Stat(&stat, STATFLAG_NONAME);
                        ULONG size = stat.cbSize.LowPart;
                        result.resize(size);

                        LARGE_INTEGER pos = {0};
                        pStream->Seek(pos, STREAM_SEEK_SET, NULL);
                        ULONG read = 0;
                        pStream->Read(result.data(), size, &read);
                    }
                }
                pFrame->Release();
            }
        }
        pEncoder->Release();
    }
    pStream->Release();

    if (result.empty()) return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Failed to encode PNG", (int)hr});
    return result;
}

std::future<Domain::Expected<Domain::ThumbnailImage>> WinThumbnailProvider::GetThumbnailAsync(const Domain::Path& path, int targetSize) {
    return std::async(std::launch::async, [pathString = path.ToString(), targetSize]() -> Domain::Expected<Domain::ThumbnailImage> {
        HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        bool coInit = SUCCEEDED(hr);

        auto cleanup = [&coInit]() {
            if (coInit) CoUninitialize();
        };

        std::wstring wPath = WinPathHelper::NormalizeAndResolvePath(pathString);
        if (wPath.empty()) {
            cleanup();
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty path", 0});
        }

        IWICImagingFactory* pFactory = NULL;
        hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
        if (FAILED(hr)) {
            cleanup();
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Failed to create WIC factory", (int)hr});
        }

        Domain::Expected<std::vector<uint8_t>> rawBytes = Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Unknown error", 0});
        UINT actWidth = 0, actHeight = 0;

        IShellItem* pItem = NULL;
        hr = SHCreateItemFromParsingName(wPath.c_str(), NULL, IID_PPV_ARGS(&pItem));
        if (SUCCEEDED(hr)) {
            IShellItemImageFactory* pImageFactory = NULL;
            hr = pItem->QueryInterface(IID_PPV_ARGS(&pImageFactory));
            if (SUCCEEDED(hr)) {
                HBITMAP hBitmap = NULL;
                SIZE sz = { targetSize, targetSize };
                hr = pImageFactory->GetImage(sz, SIIGBF_RESIZETOFIT | SIIGBF_THUMBNAILONLY, &hBitmap);
                if (SUCCEEDED(hr)) {
                    IWICBitmap* pWicBitmap = NULL;
                    hr = pFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, &pWicBitmap);
                    if (SUCCEEDED(hr)) {
                        pWicBitmap->GetSize(&actWidth, &actHeight);
                        rawBytes = SaveWICBitmapToPNG(pFactory, pWicBitmap);
                        pWicBitmap->Release();
                    }
                    DeleteObject(hBitmap);
                }
                pImageFactory->Release();
            }
            pItem->Release();
        }

        if (!rawBytes.has_value()) {
            SHFILEINFOW wInfo = {0};
            if (SHGetFileInfoW(wPath.c_str(), 0, &wInfo, sizeof(wInfo), SHGFI_SYSICONINDEX)) {
                IImageList* pImageList = nullptr;
                HRESULT hrList = SHGetImageList(SHIL_JUMBO, IID_PPV_ARGS(&pImageList));
                if (SUCCEEDED(hrList) && pImageList) {
                    HICON hIcon = nullptr;
                    HRESULT hrIcon = pImageList->GetIcon(wInfo.iIcon, ILD_TRANSPARENT, &hIcon);
                    if (SUCCEEDED(hrIcon) && hIcon) {
                        IWICBitmap* pWicBitmap = NULL;
                        HRESULT hrBmp = pFactory->CreateBitmapFromHICON(hIcon, &pWicBitmap);
                        if (SUCCEEDED(hrBmp)) {
                            pWicBitmap->GetSize(&actWidth, &actHeight);
                            rawBytes = SaveWICBitmapToPNG(pFactory, pWicBitmap);
                            pWicBitmap->Release();
                        }
                        DestroyIcon(hIcon);
                    }
                    pImageList->Release();
                }
            }
        }

        pFactory->Release();
        cleanup();
        
        if (!rawBytes.has_value()) {
             return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Failed to extract thumbnail or icon", 0});
        }

        return Domain::ThumbnailImage{rawBytes.value(), static_cast<int>(actWidth), static_cast<int>(actHeight)};
    });
}

} // namespace ExplorerX::Platform
