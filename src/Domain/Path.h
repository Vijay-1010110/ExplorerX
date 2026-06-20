#pragma once
#include <string>

namespace ExplorerX::Domain {

class Path {
public:
    Path() = default;
    explicit Path(std::string pathString) : m_pathString(std::move(pathString)) {}

    const std::string& ToString() const { return m_pathString; }

private:
    std::string m_pathString;
};

} // namespace ExplorerX::Domain
