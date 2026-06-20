add_executable(ExplorerX
    main.cpp
    MainWindow.cpp
    MainWindow.h
    Views/DirectoryTreeView.h
    Views/DirectoryTreeView.cpp
    Views/FileGridView.h
    Views/FileGridView.cpp
)

target_link_libraries(ExplorerX
    PRIVATE
        Domain
        Core
        Qt6::Widgets
)
