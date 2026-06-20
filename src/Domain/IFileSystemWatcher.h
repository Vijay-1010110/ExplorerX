#pragma once
#include "Path.h"
#include <functional>

namespace ExplorerX::Domain {

enum class FileSystemEventType {
    Created,
    Deleted,
    Modified,
    Renamed
};

struct FileSystemEvent {
    FileSystemEventType Type;
    Path TargetPath;
    Path OldPath; // Valid only when Type is Renamed
};

class IFileSystemWatcher {
public:
    virtual ~IFileSystemWatcher() = default;

    // Subscribes to file system events for a specific directory path.
    // The callback will be invoked asynchronously when an event occurs.
    virtual void StartWatching(const Path& directoryPath, std::function<void(const FileSystemEvent&)> callback) = 0;

    // Stops watching and unregisters the callback.
    virtual void StopWatching() = 0;
};

} // namespace ExplorerX::Domain
