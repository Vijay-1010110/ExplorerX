add_library(Core
    FileOperationOrchestrator.cpp
    FileOperationOrchestrator.h
    IndexOrchestrator.cpp
    IndexOrchestrator.h
    ThumbnailOrchestrator.cpp
    ThumbnailOrchestrator.h
    AIIntentOrchestrator.cpp
    AIIntentOrchestrator.h
    ThemeManager.cpp
    ThemeManager.h
)

target_include_directories(Core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

find_package(Qt6 COMPONENTS Core REQUIRED)
set_target_properties(Core PROPERTIES AUTOMOC ON)

target_link_libraries(Core PUBLIC Domain Qt6::Core)
target_compile_features(Core PUBLIC cxx_std_20)
