#pragma once
#include <QTableView>

class FileGridView : public QTableView {
    Q_OBJECT
public:
    explicit FileGridView(QWidget *parent = nullptr);
    ~FileGridView() override;

private:
    void setupDummyModel();
};
