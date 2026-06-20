#pragma once
#include "Error.h"

// Check if std::expected is available, otherwise use tl::expected
#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#include <expected>
namespace ExplorerX::Domain {
    template <typename T>
    using Expected = std::expected<T, Error>;
}
#else
#include <tl/expected.hpp>
namespace ExplorerX::Domain {
    template <typename T>
    using Expected = tl::expected<T, Error>;
}
#endif
