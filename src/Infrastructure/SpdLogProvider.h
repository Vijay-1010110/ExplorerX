#pragma once

#include "ILogger.h"
#include <memory>
#include <string_view>

namespace spdlog {
    class logger;
}

namespace ExplorerX::Infrastructure {

class SpdLogProvider : public Domain::ILogger {
public:
    SpdLogProvider();
    ~SpdLogProvider() override;

    void Info(std::string_view message) override;
    void Warning(std::string_view message) override;
    void Error(std::string_view message) override;
    void Debug(std::string_view message) override;

private:
    std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace ExplorerX::Infrastructure
