#pragma once

#include "../Domain/ILLMClient.h"
#include <QString>

namespace ExplorerX::Platform {

class GeminiLLMClient : public Domain::ILLMClient {
public:
    GeminiLLMClient();
    ~GeminiLLMClient() override = default;

    std::future<Domain::Expected<Domain::AIIntent>> ParseIntent(const std::string& prompt) override;

private:
    QString GetApiKey() const;
};

} // namespace ExplorerX::Platform
