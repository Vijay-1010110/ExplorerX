# API Contracts

Every module talks through interfaces, not concrete classes. This is critical for future-proofing and swapping out implementations (e.g., UI or filesystem backends).

## Core File System Provider
```cpp
struct IFileSystemProvider {
    virtual Expected<ListingResult> Enumerate(const Path& path) = 0;
    virtual Expected<void> Copy(const CopyRequest& req) = 0;
    virtual Expected<void> Move(const MoveRequest& req) = 0;
    virtual Expected<void> Delete(const DeleteRequest& req) = 0;
};
```

## Error Model
Define structured errors instead of throwing exceptions across boundaries:
- access denied
- path not found
- sharing violation
- network unavailable
- unsupported reparse point
- stale cache
- operation cancelled
- disk full
- filename invalid
- long path issue

## Result Model
Use explicit success/failure results (e.g., `Expected<T>`).

## Plugin Contract
For context menu plugins, preview plugins, search providers, etc.
- versioned interfaces
- capability declarations
- sandboxing where possible
- permission declarations
- load/unload lifecycle
- failure isolation

A bad plugin must not crash the host. Plugins must be disable-able and have compatibility versions. The plugin ABI must be stable or carefully versioned.
