#include "ThemeManager.h"
#include <spdlog/spdlog.h>

namespace ExplorerX::Core {

void ThemeManager::LoadTheme(const QString& themeName) {
    spdlog::info("Loading theme: {}", themeName.toStdString());
    emit ThemeChanged(themeName);
}

void ThemeManager::SetCustomBackground(const QString& imagePath) {
    spdlog::info("Setting custom background: {}", imagePath.toStdString());
    emit BackgroundChanged(imagePath);
}

} // namespace ExplorerX::Core
