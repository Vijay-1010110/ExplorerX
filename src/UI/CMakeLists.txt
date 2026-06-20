add_executable(ExplorerX
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

target_link_libraries(ExplorerX
    PRIVATE
        Domain
        Core
        Qt6::Widgets
)
