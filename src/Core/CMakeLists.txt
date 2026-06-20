add_library(Core
    FileOperationOrchestrator.cpp
    FileOperationOrchestrator.h
    IndexOrchestrator.cpp
    IndexOrchestrator.h
)

target_include_directories(Core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(Core PUBLIC Domain)
target_compile_features(Core PUBLIC cxx_std_20)
