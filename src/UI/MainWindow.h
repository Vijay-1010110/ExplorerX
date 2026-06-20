#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCopy();
    void onPaste();
    void onCut();
    void onDelete();

private:
    void setupUi();
    void setupActions();
};
