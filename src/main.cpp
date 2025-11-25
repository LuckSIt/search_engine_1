#include <iostream>
#include "nlohmann/json.hpp"
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <fstream>

using json = nlohmann::json;

int main() {
    try {
        std::cout << "Starting..." << std::endl;

        ConverterJSON converter;

        int max_responses = converter.GetResponseLimit();

        std::cout << "Loading documents..." << std::endl;
        std::vector<std::string> documents = converter.GetTextDocuments();
        
        if (documents.empty()) {
            std::cerr << "Warning: No documents found" << std::endl;
            return 1;
        }
        
        std::cout << "Documents loaded: " << documents.size() << std::endl;

        std::cout << "Building index..." << std::endl;
        InvertedIndex index;
        index.UpdateDocumentBase(documents);
        std::cout << "Index built successfully" << std::endl;

        std::cout << "Loading requests..." << std::endl;
        std::vector<std::string> requests = converter.GetRequests();
        
        if (requests.empty()) {
            std::cout << "No search requests found" << std::endl;
            return 0;
        }
        
        std::cout << "Requests loaded: " << requests.size() << std::endl;

        SearchServer server(index);

        std::cout << "Searching..." << std::endl;
        auto search_results = server.search(requests);

        std::vector<std::vector<std::pair<int, float>>> answers;
        
        for (const auto& result : search_results) {
            std::vector<std::pair<int, float>> answer;

            size_t count = std::min(result.size(), static_cast<size_t>(max_responses));
            
            for (size_t i = 0; i < count; ++i) {
                answer.push_back({static_cast<int>(result[i].doc_id), result[i].rank});
            }
            
            answers.push_back(answer);
        }

        std::cout << "Saving results..." << std::endl;
        converter.putAnswers(answers);
        
        std::cout << "Search completed successfully!" << std::endl;
        std::cout << "Results saved to answers.json" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
