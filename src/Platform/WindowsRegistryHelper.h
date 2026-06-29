#pragma once

#include <QString>
#include <QIcon>
#include <vector>

namespace ExplorerX::Platform {

struct ShellNewTemplate {
    QString extension;
    QString friendlyName;
    QIcon icon;
};

class WindowsRegistryHelper {
public:
    static std::vector<ShellNewTemplate> GetShellNewTemplates();
};

} // namespace ExplorerX::Platform
