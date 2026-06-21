#include "LLMClient.h"
#include <windows.h>
#include <winhttp.h>
#include <nlohmann/json.hpp>
#include <sstream>

#pragma comment(lib, "winhttp.lib")

using json = nlohmann::json;

namespace ExplorerX::Platform {

LLMClient::LLMClient(const std::string& apiKey, const std::string& endpoint)
    : m_apiKey(apiKey), m_endpoint(endpoint)
{
}

std::future<Domain::Expected<Domain::AIIntent>> LLMClient::ParseIntent(const std::string& prompt) {
    return std::async(std::launch::async, [this, prompt]() -> Domain::Expected<Domain::AIIntent> {
        json payload;
        payload["model"] = "gpt-4";
        payload["messages"] = json::array({
            {{"role", "system"}, {"content", "You are a filesystem intent parser. Respond ONLY in valid JSON. Structure: {\"action\": \"Copy|Move|Delete|Search|Unknown\", \"targetPaths\": [\"path1\"], \"destinationPath\": \"dest\", \"searchQuery\": \"query\"}"}},
            {{"role", "user"}, {"content", prompt}}
        });

        std::string payloadStr = payload.dump();

        URL_COMPONENTS urlComp = {0};
        urlComp.dwStructSize = sizeof(urlComp);
        
        std::wstring wEndpoint(m_endpoint.begin(), m_endpoint.end());
        
        wchar_t hostName[256];
        wchar_t urlPath[1024];
        urlComp.lpszHostName = hostName;
        urlComp.dwHostNameLength  = sizeof(hostName) / sizeof(hostName[0]);
        urlComp.lpszUrlPath = urlPath;
        urlComp.dwUrlPathLength   = sizeof(urlPath) / sizeof(urlPath[0]);

        if (!WinHttpCrackUrl(wEndpoint.c_str(), 0, 0, &urlComp)) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "Invalid endpoint URL", 0});
        }

        HINTERNET hSession = WinHttpOpen(L"ExplorerX/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "WinHttpOpen failed", 0});

        HINTERNET hConnect = WinHttpConnect(hSession, hostName, urlComp.nPort, 0);
        if (!hConnect) { WinHttpCloseHandle(hSession); return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "WinHttpConnect failed", 0}); }

        DWORD dwOpenRequestFlag = (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwOpenRequestFlag);
        if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "WinHttpOpenRequest failed", 0}); }

        std::wstring headers = L"Content-Type: application/json\r\nAuthorization: Bearer ";
        std::wstring wApiKey(m_apiKey.begin(), m_apiKey.end());
        headers += wApiKey;

        if (!WinHttpSendRequest(hRequest, headers.c_str(), -1, (LPVOID)payloadStr.c_str(), (DWORD)payloadStr.length(), (DWORD)payloadStr.length(), 0)) {
            WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "WinHttpSendRequest failed", 0});
        }

        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "WinHttpReceiveResponse failed", 0});
        }

        DWORD statusCode = 0;
        DWORD dwSize = sizeof(statusCode);
        WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);

        std::string response;
        DWORD dwDownloaded = 0;
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;
            std::vector<char> buffer(dwSize + 1, 0);
            if (!WinHttpReadData(hRequest, (LPVOID)buffer.data(), dwSize, &dwDownloaded)) break;
            response.append(buffer.data(), dwDownloaded);
        } while (dwSize > 0);

        WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);

        if (statusCode != 200) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::Unknown, "LLM API returned non-200 status", 0});
        }

        try {
            auto j = json::parse(response);
            std::string contentStr = j["choices"][0]["message"]["content"].get<std::string>();
            auto contentJ = json::parse(contentStr);

            Domain::AIIntent intent;
            std::string actionStr = contentJ.value("action", "Unknown");
            if (actionStr == "Copy") intent.Action = Domain::AIAction::Copy;
            else if (actionStr == "Move") intent.Action = Domain::AIAction::Move;
            else if (actionStr == "Delete") intent.Action = Domain::AIAction::Delete;
            else if (actionStr == "Search") intent.Action = Domain::AIAction::Search;
            else intent.Action = Domain::AIAction::Unknown;

            if (contentJ.contains("targetPaths")) {
                for (auto& p : contentJ["targetPaths"]) {
                    intent.TargetPaths.push_back(Domain::Path(p.get<std::string>()));
                }
            }

            intent.DestinationPath = Domain::Path(contentJ.value("destinationPath", ""));
            intent.SearchQuery = contentJ.value("searchQuery", "");

            return intent;
        } catch (const std::exception& e) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Failed to parse LLM response", 0});
        }
    });
}

} // namespace ExplorerX::Platform
