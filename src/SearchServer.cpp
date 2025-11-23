#include "../include/SearchServer.h"
#include <sstream>
#include <algorithm>
#include <set>
#include <cmath>

std::vector<std::string> splitQueryIntoWords(const std::string& query) {
    std::vector<std::string> words;
    std::istringstream iss(query);
    std::string word;
    
    while (iss >> word) {
        words.push_back(word);
    }
    
    return words;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> results;
    
    for (const auto& query : queries_input) {
        std::vector<std::string> query_words = splitQueryIntoWords(query);
        
        if (query_words.empty()) {
            results.push_back({});
            continue;
        }

        std::set<std::string> unique_words(query_words.begin(), query_words.end());
        std::vector<std::string> unique_words_vec(unique_words.begin(), unique_words.end());

        std::vector<std::pair<std::string, size_t>> word_frequencies;
        for (const auto& word : unique_words_vec) {
            auto entries = _index.GetWordCount(word);
            size_t total_count = 0;
            for (const auto& entry : entries) {
                total_count += entry.count;
            }
            word_frequencies.push_back({word, total_count});
        }

        std::sort(word_frequencies.begin(), word_frequencies.end(),
                  [](const auto& a, const auto& b) {
                      return a.second < b.second;
                  });

        std::set<size_t> candidate_docs;
        
        if (!word_frequencies.empty()) {
            auto first_word_entries = _index.GetWordCount(word_frequencies[0].first);
            for (const auto& entry : first_word_entries) {
                candidate_docs.insert(entry.doc_id);
            }

            for (size_t i = 1; i < word_frequencies.size(); ++i) {
                auto word_entries = _index.GetWordCount(word_frequencies[i].first);
                std::set<size_t> word_docs;
                for (const auto& entry : word_entries) {
                    word_docs.insert(entry.doc_id);
                }

                std::set<size_t> intersection;
                std::set_intersection(candidate_docs.begin(), candidate_docs.end(),
                                     word_docs.begin(), word_docs.end(),
                                     std::inserter(intersection, intersection.begin()));
                candidate_docs = intersection;

                if (candidate_docs.empty()) {
                    break;
                }
            }
        }

        if (candidate_docs.empty()) {
            results.push_back({});
            continue;
        }

        std::vector<RelativeIndex> relevance;
        
        for (size_t doc_id : candidate_docs) {
            float absolute_relevance = 0.0f;
            
            for (const auto& [word, _] : word_frequencies) {
                auto entries = _index.GetWordCount(word);
                for (const auto& entry : entries) {
                    if (entry.doc_id == doc_id) {
                        absolute_relevance += static_cast<float>(entry.count);
                        break;
                    }
                }
            }
            
            relevance.push_back({doc_id, absolute_relevance});
        }

        float max_relevance = 0.0f;
        for (const auto& rel : relevance) {
            if (rel.rank > max_relevance) {
                max_relevance = rel.rank;
            }
        }

        if (max_relevance > 0.0f) {
            for (auto& rel : relevance) {
                rel.rank = rel.rank / max_relevance;
            }
        }

        std::sort(relevance.begin(), relevance.end(),
                  [](const RelativeIndex& a, const RelativeIndex& b) {
                      if (std::abs(a.rank - b.rank) < 0.0001f) {
                          return a.doc_id < b.doc_id;
                      }
                      return a.rank > b.rank;
                  });
        
        results.push_back(relevance);
    }
    
    return results;
}

