#include "InvertedIndex.h"
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>

std::vector<std::string> splitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream iss(text);
    std::string word;
    
    while (iss >> word) {
        words.push_back(word);
    }
    
    return words;
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    docs.clear();
    freq_dictionary.clear();

    docs = input_docs;

    std::mutex dict_mutex;

    auto indexDocument = [this, &dict_mutex](size_t doc_id) {
        if (doc_id >= docs.size()) return;

        std::vector<std::string> words = splitIntoWords(docs[doc_id]);

        std::map<std::string, size_t> word_count;
        for (const auto& word : words) {
            word_count[word]++;
        }

        std::lock_guard<std::mutex> lock(dict_mutex);
        for (const auto& [word, count] : word_count) {
            auto it = freq_dictionary.find(word);
            if (it != freq_dictionary.end()) {
                bool found = false;
                for (auto& entry : it->second) {
                    if (entry.doc_id == doc_id) {
                        entry.count = count;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    it->second.push_back({doc_id, count});
                }
            } else {
                freq_dictionary[word] = {{doc_id, count}};
            }
        }
    };

    std::vector<std::thread> threads;
    for (size_t i = 0; i < docs.size(); ++i) {
        threads.emplace_back(indexDocument, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    auto it = freq_dictionary.find(word);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}

