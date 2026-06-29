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
        spdlog::spdlog
)
