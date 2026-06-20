cmake_minimum_required(VERSION 3.20)
project(ExplorerX VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# FetchContent for tl::expected
include(FetchContent)
FetchContent_Declare(
    tl_expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
    GIT_TAG master # Or a specific release tag like v1.1.0
)
FetchContent_MakeAvailable(tl_expected)

# FetchContent for spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

add_subdirectory(src)
