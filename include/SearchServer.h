#ifndef SEARCH_ENGINE_1_SEARCHSERVER_H
#define SEARCH_ENGINE_1_SEARCHSERVER_H

#include <vector>
#include <string>
#include "InvertedIndex.h"

class SearchServer {
public:
    SearchServer(InvertedIndex& idx) : _index(idx){ };
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);
private:
    InvertedIndex& _index;
};

#endif //SEARCH_ENGINE_1_SEARCHSERVER_H
