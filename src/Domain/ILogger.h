#pragma once

#include <string_view>

namespace ExplorerX::Domain {

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void Info(std::string_view message) = 0;
    virtual void Warning(std::string_view message) = 0;
    virtual void Error(std::string_view message) = 0;
    virtual void Debug(std::string_view message) = 0;
};

} // namespace ExplorerX::Domain
