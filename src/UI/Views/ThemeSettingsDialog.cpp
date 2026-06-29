#include "ThemeSettingsDialog.h"
#include "../../Core/ThemeManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>

namespace ExplorerX::UI::Views {

ThemeSettingsDialog::ThemeSettingsDialog(QWidget* parent)
    : QDialog(parent) {
    setupUi();
    
    connect(m_themeComboBox, &QComboBox::currentTextChanged, this, &ThemeSettingsDialog::onThemeChanged);
    connect(m_btnSelectBackground, &QPushButton::clicked, this, &ThemeSettingsDialog::onSelectCustomBackground);
    connect(m_btnClearBackground, &QPushButton::clicked, this, &ThemeSettingsDialog::onClearBackground);
}

ThemeSettingsDialog::~ThemeSettingsDialog() = default;

void ThemeSettingsDialog::setupUi() {
    setWindowTitle(QStringLiteral("Theme Settings"));
    setMinimumWidth(300);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Theme Selection
    QHBoxLayout* themeLayout = new QHBoxLayout();
    QLabel* themeLabel = new QLabel(QStringLiteral("Theme:"), this);
    m_themeComboBox = new QComboBox(this);
    m_themeComboBox->addItem(QStringLiteral("dark"));
    m_themeComboBox->addItem(QStringLiteral("light"));
    // TODO: Determine current theme and set index
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(m_themeComboBox);
    
    mainLayout->addLayout(themeLayout);
    
    // Background Selection
    m_btnSelectBackground = new QPushButton(QStringLiteral("Select Custom Background..."), this);
    mainLayout->addWidget(m_btnSelectBackground);
    
    m_btnClearBackground = new QPushButton(QStringLiteral("Clear Background"), this);
    mainLayout->addWidget(m_btnClearBackground);
    
    mainLayout->addStretch(1);
}

void ThemeSettingsDialog::onThemeChanged(const QString& themeName) {
    auto& themeManager = Core::ThemeManager::Instance();
    // Assuming LoadTheme returns the QSS string, and Main Window will update or we can just apply it globally here
    // But MainWindow.cpp uses qApp->setStyleSheet, so maybe we need a global way or emit a signal.
    // Wait, the prompt says "When changed, call ThemeManager::Instance().LoadTheme(value)."
    // Let's also apply it to qApp for immediate effect.
    // Actually, MainWindow has a qApp reference. I'll just load and apply it.
    extern void ApplyThemeGlobally(const QString& theme);
    // Well, I can just use qApp here:
    // qApp->setStyleSheet(themeManager.LoadTheme(themeName));
    QString qss = themeManager.LoadTheme(themeName);
    if (!qss.isEmpty() && qApp) {
        qApp->setStyleSheet(qss);
    }
}

void ThemeSettingsDialog::onSelectCustomBackground() {
    QString picturesLoc = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QString filePath = QFileDialog::getOpenFileName(this, 
        QStringLiteral("Select Background Image"), 
        picturesLoc, 
        QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp)"));
        
    if (!filePath.isEmpty()) {
        Core::ThemeManager::Instance().SetCustomBackground(filePath);
    }
}

void ThemeSettingsDialog::onClearBackground() {
    Core::ThemeManager::Instance().SetCustomBackground("");
}

} // namespace ExplorerX::UI::Views
