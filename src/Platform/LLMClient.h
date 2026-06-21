#pragma once
#include "../Domain/ILLMClient.h"

namespace ExplorerX::Platform {

class LLMClient : public Domain::ILLMClient {
public:
    // apiKey can be passed via environment or config, defaults to dummy for now
    LLMClient(const std::string& apiKey = "sk-dummy-key", const std::string& endpoint = "https://api.openai.com/v1/chat/completions");
    ~LLMClient() override = default;

    std::future<Domain::Expected<Domain::AIIntent>> ParseIntent(const std::string& prompt) override;

private:
    std::string m_apiKey;
    std::string m_endpoint;
};

} // namespace ExplorerX::Platform
