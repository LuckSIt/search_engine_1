add_executable(search_engine_tests
        test_inverted.cpp
    test_search_server.cpp
)

target_link_libraries(search_engine_tests PRIVATE
    search_engine_lib
    gtest_main
)

target_include_directories(search_engine_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(search_engine_tests)

