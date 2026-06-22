#pragma once
#include <QObject>
#include <QString>

namespace ExplorerX::Core {

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& Instance() {
        static ThemeManager instance;
        return instance;
    }

    void LoadTheme(const QString& themeName);
    void SetCustomBackground(const QString& imagePath);

signals:
    void ThemeChanged(const QString& themeName);
    void BackgroundChanged(const QString& imagePath);

private:
    ThemeManager() = default;
    ~ThemeManager() override = default;

    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
};

} // namespace ExplorerX::Core
