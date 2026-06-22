#include "ThemeManager.h"
#include <spdlog/spdlog.h>
#include <QFile>
#include <QTextStream>

namespace ExplorerX::Core {

QString ThemeManager::LoadTheme(const QString& themeName) {
    spdlog::info("Loading theme: {}", themeName.toStdString());
    emit ThemeChanged(themeName);
    
    QFile file(":/Themes/" + themeName + ".qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        return stream.readAll();
    }
    return QString();
}

void ThemeManager::SetCustomBackground(const QString& imagePath) {
    spdlog::info("Setting custom background: {}", imagePath.toStdString());
    m_customBackground = imagePath;
    emit BackgroundChanged(imagePath);
}

} // namespace ExplorerX::Core
