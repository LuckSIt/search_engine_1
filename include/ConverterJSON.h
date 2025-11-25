#ifndef SEARCH_ENGINE_1_CONVERTERJSON_H
#define SEARCH_ENGINE_1_CONVERTERJSON_H
#pragma once
#include <vector>
#include <string>

class ConverterJSON {
public:
    ConverterJSON() = default;
    std::vector<std::string> GetTextDocuments();
    int GetResponseLimit();
    std::vector<std::string> GetRequests();
    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);
};

#endif //SEARCH_ENGINE_1_CONVERTERJSON_H
