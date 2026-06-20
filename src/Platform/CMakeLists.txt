add_library(Platform
    WinFileSystemProvider.cpp
    WinFileSystemProvider.h
    WinPathHelper.cpp
    WinPathHelper.h
    WinFileSystemWatcher.cpp
    WinFileSystemWatcher.h
)

target_link_libraries(Platform
    PUBLIC
        Domain
)
