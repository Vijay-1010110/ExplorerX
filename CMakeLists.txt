cmake_minimum_required(VERSION 3.20)
project(ExplorerX VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(tl-expected CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
set(CMAKE_PREFIX_PATH "D:/Antigravity projects/ExplorerX/external/Qt/6.6.1/msvc2019_64" ${CMAKE_PREFIX_PATH})
find_package(Qt6 COMPONENTS Core Gui Widgets Network REQUIRED)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
