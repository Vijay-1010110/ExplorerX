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
    NotFound,
    InvalidFormat,
    Unknown
};

struct Error {
    ErrorCode Code;
    std::string Message;
    int NativeErrorCode = 0; 
};

} // namespace ExplorerX::Domain
