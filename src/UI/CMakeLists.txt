set(CMAKE_AUTORCC ON)

add_executable(ExplorerX
    icons.qrc
    themes.qrc
    main.cpp
    MainWindow.cpp
    MainWindow.h
    Views/DirectoryTreeView.h
    Views/DirectoryTreeView.cpp
    Views/FileGridView.h
    Views/FileGridView.cpp
    ViewModels/FileItemModel.h
    ViewModels/FileItemModel.cpp
    ViewModels/DirectoryItemModel.h
    ViewModels/DirectoryItemModel.cpp
    Views/ThemeSettingsDialog.h
    Views/ThemeSettingsDialog.cpp
)

target_link_libraries(ExplorerX
    PRIVATE
        Domain
        Core
        Platform
        Infrastructure
        Qt6::Widgets
        Qt6::Network
        Qt6::Svg
        spdlog::spdlog
)

# Qt Deployment (windeployqt)
if(WIN32)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ExplorerX POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --no-compiler-runtime
            "$<TARGET_FILE:ExplorerX>"
            COMMENT "Deploying Qt libraries using windeployqt..."
        )
    endif()
endif()
