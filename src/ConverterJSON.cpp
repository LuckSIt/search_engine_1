#include "../include/ConverterJSON.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

using json = nlohmann::json;

int ConverterJSON::GetResponseLimit() {
    std::ifstream config_file("config.json");
    if (!config_file.is_open()) {
        throw std::runtime_error("config file is missing");
    }
    
    json config = json::parse(config_file);
    config_file.close();

    if (!config.contains("config")) {
        throw std::runtime_error("config file is empty");
    }

    if (config["config"].contains("max_responses")) {
        return config["config"]["max_responses"];
    } else if (config["config"].contains("max_response")) {
        return config["config"]["max_response"];
    } else {
        return 5;
    }
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> documents;
    std::ifstream config_file("config.json");
    
    if (!config_file.is_open()) {
        throw std::runtime_error("config file is missing");
    }
    
    json config = json::parse(config_file);
    config_file.close();
    
    if (!config.contains("config")) {
        throw std::runtime_error("config file is empty");
    }

    if (!config.contains("files")) {
        return documents;
    }

    for (const auto& file_path : config["files"]) {
        std::string path = file_path.get<std::string>();
        std::ifstream file(path);
        
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            documents.push_back(buffer.str());
            file.close();
        } else {
            std::cerr << "Warning: File not found: " << path << std::endl;
        }
    }
    
    return documents;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::vector<std::string> requests;
    std::ifstream requests_file("requests.json");
    
    if (!requests_file.is_open()) {
        return requests;
    }
    
    json requests_json = json::parse(requests_file);
    requests_file.close();
    
    if (requests_json.contains("requests")) {
        for (const auto& request : requests_json["requests"]) {
            requests.push_back(request.get<std::string>());
        }
    }
    
    return requests;
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
    json result;
    json answers_obj;

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string request_id = "request" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1);
        json request_result;
        
        if (answers[i].empty()) {
            request_result["result"] = "false";
        } else {
            request_result["result"] = "true";
            
            if (answers[i].size() > 1) {
                json relevance_array = json::array();
                for (const auto& pair : answers[i]) {
                    json doc_entry;
                    doc_entry["docid"] = pair.first;
                    doc_entry["rank"] = pair.second;
                    relevance_array.push_back(doc_entry);
                }
                request_result["relevance"] = relevance_array;
            } else {
                request_result["docid"] = answers[i][0].first;
                request_result["rank"] = answers[i][0].second;
            }
        }
        
        answers_obj[request_id] = request_result;
    }
    
    result["answers"] = answers_obj;

    std::ofstream answers_file("answers.json");
    if (answers_file.is_open()) {
        answers_file << result.dump(4);
        answers_file.close();
    } else {
        throw std::runtime_error("Cannot write to answers.json");
    }
}