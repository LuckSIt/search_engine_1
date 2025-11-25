add_library(search_engine_lib
    ConverterJSON.cpp
    InvertedIndex.cpp
    SearchServer.cpp
)

target_include_directories(search_engine_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(search_engine_lib PUBLIC
    nlohmann_json::nlohmann_json
)

add_executable(search_engine_1 main.cpp)

target_link_libraries(search_engine_1 PRIVATE search_engine_lib)


