add_library(Platform
    WinFileSystemProvider.cpp
    WinFileSystemProvider.h
)

target_link_libraries(Platform
    PUBLIC
        Domain
)
