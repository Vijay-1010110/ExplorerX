#include "FileOperationOrchestrator.h"

namespace ExplorerX::Core {

FileOperationOrchestrator::FileOperationOrchestrator(std::shared_ptr<Domain::IFileSystemProvider> fsProvider)
    : m_fsProvider(std::move(fsProvider)) {}

std::future<Domain::Expected<void>> FileOperationOrchestrator::CopyBatch(
    const std::vector<Domain::CopyRequest>& requests,
    const Domain::ProgressContext& progressContext) 
{
    return std::async(std::launch::async, [this, requests, progressContext]() -> Domain::Expected<void> {
        for (const auto& req : requests) {
            if (progressContext.IsCancelled && progressContext.IsCancelled->load()) {
                return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::OperationCancelled, "Operation cancelled by user" });
            }
            
            auto future = m_fsProvider->Copy(req, progressContext);
            auto result = future.get();
            if (!result) {
                return result; 
            }
        }
        return {};
    });
}

std::future<Domain::Expected<void>> FileOperationOrchestrator::MoveBatch(
    const std::vector<Domain::MoveRequest>& requests,
    const Domain::ProgressContext& progressContext) 
{
    return std::async(std::launch::async, [this, requests, progressContext]() -> Domain::Expected<void> {
        for (const auto& req : requests) {
            if (progressContext.IsCancelled && progressContext.IsCancelled->load()) {
                return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::OperationCancelled, "Operation cancelled by user" });
            }
            
            auto future = m_fsProvider->Move(req, progressContext);
            auto result = future.get();
            if (!result) {
                return result;
            }
        }
        return {};
    });
}

std::future<Domain::Expected<void>> FileOperationOrchestrator::DeleteBatch(
    const std::vector<Domain::DeleteRequest>& requests,
    const Domain::ProgressContext& progressContext) 
{
    return std::async(std::launch::async, [this, requests, progressContext]() -> Domain::Expected<void> {
        for (const auto& req : requests) {
            if (progressContext.IsCancelled && progressContext.IsCancelled->load()) {
                return Domain::MakeUnexpected(Domain::Error{ Domain::ErrorCode::OperationCancelled, "Operation cancelled by user" });
            }
            
            auto future = m_fsProvider->Delete(req, progressContext);
            auto result = future.get();
            if (!result) {
                return result;
            }
        }
        return {};
    });
}

} // namespace ExplorerX::Core
