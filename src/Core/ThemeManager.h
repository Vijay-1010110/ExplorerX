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

    QString LoadTheme(const QString& themeName);
    void SetCustomBackground(const QString& imagePath);
    QString GetCustomBackground() const { return m_customBackground; }

signals:
    void ThemeChanged(const QString& themeName);
    void BackgroundChanged(const QString& imagePath);

private:
    ThemeManager() = default;
    ~ThemeManager() override = default;

    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    QString m_customBackground;
};

} // namespace ExplorerX::Core
