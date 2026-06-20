add_library(Infrastructure
    SpdLogProvider.cpp
    SpdLogProvider.h
)

target_link_libraries(Infrastructure
    PUBLIC
        Domain
    PRIVATE
        spdlog::spdlog
)
