#include "JsonConfigProvider.h"
#include <fstream>

namespace ExplorerX::Infrastructure {

JsonConfigProvider::JsonConfigProvider(std::filesystem::path configPath)
    : m_configPath(std::move(configPath)) {
    auto _ = Load();
}

JsonConfigProvider::~JsonConfigProvider() {
    auto _ = Save();
}

Domain::Expected<void> JsonConfigProvider::Load() {
    std::lock_guard lock(m_mutex);
    if (!std::filesystem::exists(m_configPath)) {
        m_config = nlohmann::json::object();
        return {};
    }

    try {
        std::ifstream file(m_configPath);
        if (!file.is_open()) {
            return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::AccessDenied, "Failed to open config file for reading" });
        }
        file >> m_config;
        return {};
    } catch (const std::exception& e) {
        return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::InvalidFormat, e.what() });
    }
}

Domain::Expected<std::string> JsonConfigProvider::GetString(std::string_view key) const {
    std::lock_guard lock(m_mutex);
    std::string k(key);
    if (m_config.contains(k) && m_config[k].is_string()) {
        return m_config[k].get<std::string>();
    }
    return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::NotFound, "Key not found or not a string" });
}

Domain::Expected<int> JsonConfigProvider::GetInt(std::string_view key) const {
    std::lock_guard lock(m_mutex);
    std::string k(key);
    if (m_config.contains(k) && m_config[k].is_number_integer()) {
        return m_config[k].get<int>();
    }
    return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::NotFound, "Key not found or not an integer" });
}

Domain::Expected<bool> JsonConfigProvider::GetBool(std::string_view key) const {
    std::lock_guard lock(m_mutex);
    std::string k(key);
    if (m_config.contains(k) && m_config[k].is_boolean()) {
        return m_config[k].get<bool>();
    }
    return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::NotFound, "Key not found or not a boolean" });
}

Domain::Expected<void> JsonConfigProvider::SetString(std::string_view key, std::string_view value) {
    std::lock_guard lock(m_mutex);
    m_config[std::string(key)] = std::string(value);
    return {};
}

Domain::Expected<void> JsonConfigProvider::SetInt(std::string_view key, int value) {
    std::lock_guard lock(m_mutex);
    m_config[std::string(key)] = value;
    return {};
}

Domain::Expected<void> JsonConfigProvider::SetBool(std::string_view key, bool value) {
    std::lock_guard lock(m_mutex);
    m_config[std::string(key)] = value;
    return {};
}

Domain::Expected<void> JsonConfigProvider::Save() {
    std::lock_guard lock(m_mutex);
    try {
        std::ofstream file(m_configPath);
        if (!file.is_open()) {
            return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::AccessDenied, "Failed to open config file for writing" });
        }
        file << m_config.dump(4);
        return {};
    } catch (const std::exception& e) {
        return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::AccessDenied, e.what() });
    }
}

} // namespace ExplorerX::Infrastructure
