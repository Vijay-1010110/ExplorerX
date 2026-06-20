# TestAgent: Populate test targets here

add_executable(ExplorerXTests 
    Infrastructure/SqliteSearchEngineTests.cpp
)

target_link_libraries(ExplorerXTests PRIVATE 
    GTest::gtest 
    GTest::gmock 
    GTest::gmock_main
    GTest::gmock_main 
    Domain 
    Core 
    Infrastructure
)

include(GoogleTest)
gtest_discover_tests(ExplorerXTests)
