cmake_minimum_required(VERSION 3.15)
project(search_engine_1)

set(CMAKE_CXX_STANDARD 20)

# Fetch dependencies
include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)

FetchContent_MakeAvailable(json googletest)

add_subdirectory(src)

add_subdirectory(tests)
