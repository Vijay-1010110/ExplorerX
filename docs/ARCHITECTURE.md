# ExplorerX Architecture

## 1. System Layers and Boundaries

ExplorerX follows a strict Clean Architecture pattern. This ensures that the application is testable, decoupled from external dependencies (like the specific UI framework or file system quirks), and maintainable.

### 1.1 Layers Overview

*   **UI Layer (Qt 6):** Located in `src/UI/`. Responsible for presentation logic, Views, and ViewModels. It strictly depends on the Application/Core layer. It must **never** directly invoke Platform or Infrastructure APIs.
*   **Application / Core Layer:** Located in `src/Core/` (or `src/Application/`). Contains application use cases and orchestrators (e.g., `FileOperationOrchestrator`, `NavigationService`). It drives the flow of data but depends entirely on the Domain layer's models and interfaces.
*   **Domain Layer:** Located in `src/Domain/`. The heart of the application. Contains pure business logic, models (`Path`, `FileItem`, `Error`), and the interface contracts (`IFileSystemProvider`, `ISearchEngine`). It has **zero dependencies** on any other layer. It must be pure C++20.
*   **Infrastructure Layer:** Located in `src/Infrastructure/`. Implements the Domain interfaces (e.g., SQLite implementations for indexing). It depends on the Domain layer for interfaces and models, and utilizes the Platform layer for system calls.
*   **Platform Layer (WinFS):** Located in `src/Platform/`. Encapsulates raw Win32, COM, and Shell APIs. This isolates all Windows-specific quirks, memory management, and types (`HANDLE`, `HRESULT`, `IShellItem`) from the rest of the application.

### 1.2 Dependency Rule

Dependencies must point **inward** toward the Domain layer:
`UI` → `Application` → `Domain` ← `Infrastructure` → `Platform`

---

## 2. Core API Contracts (Domain Interfaces)

These interfaces reside in the Domain layer and are implemented by the Infrastructure/Platform layers.

### 2.1 IFileSystemProvider

The primary contract for interacting with the underlying file system (local or network).

```cpp
#pragma once
#include "Path.h"
#include "FileItem.h"
#include "Expected.h"

namespace ExplorerX::Domain {

struct CopyRequest { Path Source; Path Destination; bool Overwrite; };
struct MoveRequest { Path Source; Path Destination; };
struct DeleteRequest { Path Target; bool Permanent; };
struct ListingResult { std::vector<FileItem> Items; };

class IFileSystemProvider {
public:
    virtual ~IFileSystemProvider() = default;

    // Asynchronous enumeration is assumed (e.g., returning a future or utilizing coroutines)
    // For the synchronous contract definition:
    virtual Expected<ListingResult> Enumerate(const Path& path) = 0;
    
    virtual Expected<void> Copy(const CopyRequest& req) = 0;
    virtual Expected<void> Move(const MoveRequest& req) = 0;
    virtual Expected<void> Delete(const DeleteRequest& req) = 0;
};

} // namespace ExplorerX::Domain
```

### 2.2 ISearchEngine

The contract for the local indexing and search system.

```cpp
#pragma once
#include "Path.h"
#include "FileItem.h"
#include "Expected.h"
#include <string>

namespace ExplorerX::Domain {

struct SearchQuery { 
    std::string Keyword; 
    Path RootPath; 
    // Filters, dates, size ranges, etc.
};
struct SearchResults { std::vector<FileItem> Matches; };

class ISearchEngine {
public:
    virtual ~ISearchEngine() = default;

    virtual Expected<void> IndexDirectory(const Path& path) = 0;
    virtual Expected<SearchResults> Query(const SearchQuery& query) = 0;
};

} // namespace ExplorerX::Domain
```

---

## 3. Result Pattern and Error Handling

Exceptions should generally be avoided across module boundaries. We use a "Result pattern" based on `std::expected` (or a C++20 backport like `tl::expected`) to explicitly handle success and failure states.

### 3.1 The Error Model

Errors must be structured, predictable, and localized.

```cpp
#pragma once
#include <string>

namespace ExplorerX::Domain {

enum class ErrorCode {
    Success = 0,
    AccessDenied,
    PathNotFound,
    SharingViolation,
    NetworkUnavailable,
    DiskFull,
    InvalidFilename,
    OperationCancelled,
    UnsupportedReparsePoint,
    Unknown
};

struct Error {
    ErrorCode Code;
    std::string Message;
    
    // Optional: Include underlying OS error codes for telemetry/diagnostics (e.g., HRESULT or GetLastError())
    int NativeErrorCode = 0; 
};

} // namespace ExplorerX::Domain
```

### 3.2 The Expected Type

Every operation that can fail returns an `Expected<T>`.

```cpp
#pragma once
#include "Error.h"
#include <expected> // C++23 standard, or utilizing a backport in C++20

namespace ExplorerX::Domain {

template <typename T>
using Expected = std::expected<T, Error>;

} // namespace ExplorerX::Domain
```

### 3.3 Example Usage

```cpp
Expected<ListingResult> result = fileSystemProvider->Enumerate(myPath);

if (result.has_value()) {
    // Proceed with result.value().Items
} else {
    // Handle error deterministically
    if (result.error().Code == ErrorCode::AccessDenied) {
        // Prompt for elevation
    }
}
```
