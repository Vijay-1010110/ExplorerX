#include "FileItemModel.h"
#include <thread>
#include <spdlog/spdlog.h>

FileItemModel::FileItemModel(std::shared_ptr<ExplorerX::Domain::IFileSystemProvider> provider,
                             std::shared_ptr<ExplorerX::Domain::ISearchEngine> searchEngine,
                             QObject *parent)
    : QAbstractTableModel(parent), m_provider(std::move(provider)), m_searchEngine(std::move(searchEngine)) {
}

FileItemModel::~FileItemModel() = default;

void FileItemModel::loadPath(const std::string& path) {
    if (!m_provider) return;

    std::thread([this, provider = m_provider, path]() {
        auto future = provider->Enumerate(ExplorerX::Domain::Path(path));
        auto result = future.get();
        if (result) {
            auto items = result.value().Items;
            QMetaObject::invokeMethod(this, [this, items = std::move(items)]() mutable {
                beginResetModel();
                m_files = std::move(items);
                endResetModel();
            });
        } else {
            spdlog::error("Failed to load path: {}", path);
        }
    }).detach();
}

void FileItemModel::performSearch(const QString& query) {
    if (!m_searchEngine) return;
    std::string q = query.toStdString();
    
    std::thread([this, engine = m_searchEngine, q]() {
        ExplorerX::Domain::SearchQuery searchQ{q, ExplorerX::Domain::Path("C:\\")};
        auto future = engine->Query(searchQ);
        auto result = future.get();
        if (result) {
            auto items = result.value().Matches;
            QMetaObject::invokeMethod(this, [this, items = std::move(items)]() mutable {
                beginResetModel();
                m_files = std::move(items);
                endResetModel();
            });
        } else {
            spdlog::error("Failed to perform search: {}", q);
        }
    }).detach();
}

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
        case 0: return QString::fromStdString(file.Name);
        case 1: return QString::number(file.Size) + " bytes"; // Formatting can be improved later
        case 2: return file.IsDirectory ? QStringLiteral("Folder") : QStringLiteral("File");
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

Qt::ItemFlags FileItemModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsDropEnabled;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}
