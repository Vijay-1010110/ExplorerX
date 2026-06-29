#include "ThemeManager.h"
#include <spdlog/spdlog.h>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>

namespace ExplorerX::Core {

ThemeManager::ThemeManager() {
    LoadConfig();
}

QString ThemeManager::GetConfigPath() const {
    return QCoreApplication::applicationDirPath() + QDir::separator() + "theme_config.json";
}

void ThemeManager::LoadConfig() {
    QFile file(GetConfigPath());
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(data));
        QJsonObject json = doc.object();
        
        if (json.contains("theme") && json["theme"].isString()) {
            m_currentTheme = json["theme"].toString();
        }
        if (json.contains("custom_background") && json["custom_background"].isString()) {
            m_customBackground = json["custom_background"].toString();
        }
    }
}

void ThemeManager::SaveConfig() {
    QJsonObject json;
    json["theme"] = m_currentTheme;
    json["custom_background"] = m_customBackground;
    
    QJsonDocument doc(json);
    QFile file(GetConfigPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    } else {
        spdlog::error("Failed to save theme config to {}", GetConfigPath().toStdString());
    }
}

QString ThemeManager::LoadTheme(const QString& themeName) {
    spdlog::info("Loading theme: {}", themeName.toStdString());
    m_currentTheme = themeName;
    SaveConfig();
    emit ThemeChanged(themeName);
    
    QFile file(":/themes/Themes/" + themeName + ".qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        return stream.readAll();
    }
    return QString();
}

void ThemeManager::SetCustomBackground(const QString& imagePath) {
    spdlog::info("Setting custom background: {}", imagePath.toStdString());
    m_customBackground = imagePath;
    SaveConfig();
    emit BackgroundChanged(imagePath);
}

} // namespace ExplorerX::Core
