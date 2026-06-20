add_library(Infrastructure
    SpdLogProvider.cpp
    SpdLogProvider.h
    JsonConfigProvider.cpp
    JsonConfigProvider.h
)

target_link_libraries(Infrastructure
    PUBLIC
        Domain
    PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        unofficial::sqlite3::sqlite3
)
