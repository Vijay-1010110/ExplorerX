add_library(Domain INTERFACE)
target_include_directories(Domain INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Domain INTERFACE tl::expected)
