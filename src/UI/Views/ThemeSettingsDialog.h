#pragma once

#include <QDialog>
#include <QComboBox>
#include <QPushButton>

namespace ExplorerX::UI::Views {

class ThemeSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ThemeSettingsDialog(QWidget* parent = nullptr);
    ~ThemeSettingsDialog() override;

private:
    void setupUi();
    void onThemeChanged(const QString& themeName);
    void onSelectCustomBackground();
    void onClearBackground();

    QComboBox* m_themeComboBox;
    QPushButton* m_btnSelectBackground;
    QPushButton* m_btnClearBackground;
};

} // namespace ExplorerX::UI::Views
