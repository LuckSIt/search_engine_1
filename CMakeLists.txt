cmake_minimum_required(VERSION 3.15)
project(search_engine_1)

set(CMAKE_CXX_STANDARD 20)
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_Declare(googletest URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz)

add_executable(search_engine_1 src/main.cpp
        include/ConverterJSON.h
        include/InvertedIndex.h
        src/ConverterJSON.cpp
        src/InvertedIndex.cpp
        include/SearchServer.h
        src/SearchServer.cpp
        tests/test_main.cpp
        tests/test_inverted.cpp
        tests/test_search_server.cpp
)

FetchContent_MakeAvailable(json googletest)

enable_testing()
target_link_libraries(search_engine_1 PRIVATE nlohmann_json::nlohmann_json gtest_main)
include(GoogleTest)
gtest_discover_tests(search_engine_1)