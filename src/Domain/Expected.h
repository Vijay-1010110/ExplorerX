#pragma once
#include "Error.h"

// Check if std::expected is available, otherwise use tl::expected
#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#include <expected>
namespace ExplorerX::Domain {
    template <typename T>
    using Expected = std::expected<T, Error>;

    inline auto MakeUnexpected(Error err) {
        return std::unexpected(std::move(err));
    }
}
#else
#include <tl/expected.hpp>
namespace ExplorerX::Domain {
    template <typename T>
    using Expected = tl::expected<T, Error>;

    inline auto MakeUnexpected(Error err) {
        return tl::make_unexpected(std::move(err));
    }
}
#endif
