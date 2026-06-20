#pragma once

#include "IConfigProvider.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <mutex>
#include <string>

namespace ExplorerX::Infrastructure {

class JsonConfigProvider : public Domain::IConfigProvider {
public:
    explicit JsonConfigProvider(std::filesystem::path configPath);
    ~JsonConfigProvider() override;

    Domain::Expected<std::string> GetString(std::string_view key) const override;
    Domain::Expected<int> GetInt(std::string_view key) const override;
    Domain::Expected<bool> GetBool(std::string_view key) const override;
    
    Domain::Expected<void> SetString(std::string_view key, std::string_view value) override;
    Domain::Expected<void> SetInt(std::string_view key, int value) override;
    Domain::Expected<void> SetBool(std::string_view key, bool value) override;
    
    Domain::Expected<void> Save() override;

private:
    Domain::Expected<void> Load();

    std::filesystem::path m_configPath;
    nlohmann::json m_config;
    mutable std::mutex m_mutex;
};

} // namespace ExplorerX::Infrastructure
