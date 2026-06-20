#include "SpdLogProvider.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <vector>

namespace ExplorerX::Infrastructure {

SpdLogProvider::SpdLogProvider() {
    spdlog::init_thread_pool(8192, 1);
    
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/explorerx.log", 1024 * 1024 * 5, 3);
    
    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    
    m_logger = std::make_shared<spdlog::async_logger>("multi_sink", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(m_logger);
    
    // Set pattern
    m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    m_logger->set_level(spdlog::level::debug);
}

SpdLogProvider::~SpdLogProvider() {
    spdlog::drop_all();
}

void SpdLogProvider::Info(std::string_view message) {
    m_logger->info(message);
}

void SpdLogProvider::Warning(std::string_view message) {
    m_logger->warn(message);
}

void SpdLogProvider::Error(std::string_view message) {
    m_logger->error(message);
}

void SpdLogProvider::Debug(std::string_view message) {
    m_logger->debug(message);
}

} // namespace ExplorerX::Infrastructure
