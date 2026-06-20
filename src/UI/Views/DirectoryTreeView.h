#pragma once
#include <QTreeView>

class DirectoryTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit DirectoryTreeView(QWidget *parent = nullptr);
    ~DirectoryTreeView() override;

private:
    void setupDummyModel();
};
