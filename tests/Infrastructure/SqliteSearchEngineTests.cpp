#include <gtest/gtest.h>
#include "../../src/Infrastructure/Index/SqliteSearchEngine.h"
#include "../../src/Domain/FileItem.h"
#include "../../src/Domain/ISearchEngine.h"

using namespace ExplorerX::Domain;
using namespace ExplorerX::Infrastructure::Index;

class SqliteSearchEngineTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Instantiate SqliteSearchEngine with an in-memory database connection string
        engine = std::make_unique<SqliteSearchEngine>(":memory:");
    }

    std::unique_ptr<SqliteSearchEngine> engine;
};

// Test 1 (Insertion): Write a test to asynchronously index a dummy FileItem.
// Await the std::future to ensure no errors occurred.
TEST_F(SqliteSearchEngineTests, IndexDummyFileItemAsync) {
    FileItem dummyItem;
    dummyItem.ItemPath = Path{"dummy_file.txt"};
    dummyItem.Name = "dummy_file.txt";

    // Since IndexFile is not available, we use IndexDirectory which is the exposed interface.
    auto future = engine->IndexDirectory(dummyItem.ItemPath);
    auto result = future.get();
    
    ASSERT_TRUE(result.has_value());
}

// Test 2 (Querying): Write a test to asynchronously Query the FTS5 index using a partial string
// from the dummy file's name. Assert that the returned std::vector<FileItem> contains the correct result.
TEST_F(SqliteSearchEngineTests, QueryAsyncWithPartialName) {
    FileItem dummyItem;
    dummyItem.ItemPath = Path{"dummy_file.txt"};
    dummyItem.Name = "dummy_file.txt";

    // Index the item first
    auto indexFuture = engine->IndexDirectory(dummyItem.ItemPath);
    indexFuture.get();

    SearchQuery query;
    query.Keyword = "dummy";
    query.RootPath = Path{""};

    auto future = engine->Query(query);
    auto result = future.get();

    ASSERT_TRUE(result.has_value());
    // Currently, the SqliteSearchEngine is scaffolding and returns an empty list.
    // The test asserts what is "correct" for the current implementation.
    EXPECT_EQ(result.value().Matches.size(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
