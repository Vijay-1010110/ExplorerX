#include "FileGridView.h"
#include <QStandardItemModel>
#include <QHeaderView>

FileGridView::FileGridView(QWidget *parent)
    : QTableView(parent) {
    setupDummyModel();
}

FileGridView::~FileGridView() = default;

void FileGridView::setupDummyModel() {
    auto *model = new QStandardItemModel(3, 3, this);
    model->setHorizontalHeaderLabels({
        QStringLiteral("Name"), 
        QStringLiteral("Size"), 
        QStringLiteral("Type")
    });

    model->setItem(0, 0, new QStandardItem(QStringLiteral("dummy_file.txt")));
    model->setItem(0, 1, new QStandardItem(QStringLiteral("12 KB")));
    model->setItem(0, 2, new QStandardItem(QStringLiteral("Text Document")));

    model->setItem(1, 0, new QStandardItem(QStringLiteral("project_report.pdf")));
    model->setItem(1, 1, new QStandardItem(QStringLiteral("2.5 MB")));
    model->setItem(1, 2, new QStandardItem(QStringLiteral("PDF File")));

    model->setItem(2, 0, new QStandardItem(QStringLiteral("holiday_photo.jpg")));
    model->setItem(2, 1, new QStandardItem(QStringLiteral("4.1 MB")));
    model->setItem(2, 2, new QStandardItem(QStringLiteral("JPEG Image")));

    setModel(model);
    
    // Stretch the name column
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}
