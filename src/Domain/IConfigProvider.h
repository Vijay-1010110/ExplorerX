#pragma once

#include "Expected.h"
#include <string_view>
#include <string>

namespace ExplorerX::Domain {

class IConfigProvider {
public:
    virtual ~IConfigProvider() = default;

    virtual Expected<std::string> GetString(std::string_view key) const = 0;
    virtual Expected<int> GetInt(std::string_view key) const = 0;
    virtual Expected<bool> GetBool(std::string_view key) const = 0;
    
    virtual Expected<void> SetString(std::string_view key, std::string_view value) = 0;
    virtual Expected<void> SetInt(std::string_view key, int value) = 0;
    virtual Expected<void> SetBool(std::string_view key, bool value) = 0;
    
    virtual Expected<void> Save() = 0;
};

} // namespace ExplorerX::Domain
