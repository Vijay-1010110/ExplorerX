#include "FileItemModel.h"

FileItemModel::FileItemModel(QObject *parent) : QAbstractTableModel(parent) {
    m_files = {
        {QStringLiteral("dummy_file.txt"), QStringLiteral("12 KB"), QStringLiteral("Text Document")},
        {QStringLiteral("project_report.pdf"), QStringLiteral("2.5 MB"), QStringLiteral("PDF File")},
        {QStringLiteral("holiday_photo.jpg"), QStringLiteral("4.1 MB"), QStringLiteral("JPEG Image")},
        {QStringLiteral("archive.zip"), QStringLiteral("150 MB"), QStringLiteral("ZIP Archive")}
    };
}

FileItemModel::~FileItemModel() = default;

int FileItemModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_files.size());
}

int FileItemModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return 3;
}

QVariant FileItemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }

    const auto& file = m_files[index.row()];
    switch (index.column()) {
        case 0: return file.name;
        case 1: return file.size;
        case 2: return file.type;
        default: return {};
    }
}

QVariant FileItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return {};
    }

    switch (section) {
        case 0: return QStringLiteral("Name");
        case 1: return QStringLiteral("Size");
        case 2: return QStringLiteral("Type");
        default: return {};
    }
}
