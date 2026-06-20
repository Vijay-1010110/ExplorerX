add_executable(ExplorerX
    main.cpp
)

target_link_libraries(ExplorerX
    PRIVATE
        Domain
        Core
        Qt6::Widgets
)
