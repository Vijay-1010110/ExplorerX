#pragma once
#include "IFileSystemProvider.h"
#include <memory>
#include <vector>

namespace ExplorerX::Core {

class FileOperationOrchestrator {
public:
    explicit FileOperationOrchestrator(std::shared_ptr<Domain::IFileSystemProvider> fsProvider);

    // Batch operations
    std::future<Domain::Expected<void>> CopyBatch(
        const std::vector<Domain::CopyRequest>& requests,
        const Domain::ProgressContext& progressContext = {});

    std::future<Domain::Expected<void>> MoveBatch(
        const std::vector<Domain::MoveRequest>& requests,
        const Domain::ProgressContext& progressContext = {});

    std::future<Domain::Expected<void>> DeleteBatch(
        const std::vector<Domain::DeleteRequest>& requests,
        const Domain::ProgressContext& progressContext = {});

private:
    std::shared_ptr<Domain::IFileSystemProvider> m_fsProvider;
};

} // namespace ExplorerX::Core
