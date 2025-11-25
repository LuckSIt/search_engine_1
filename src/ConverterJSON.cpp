#include "ConverterJSON.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

using json = nlohmann::json;

namespace {
    std::string findConfigFile(const std::string& filename) {
        std::vector<std::string> possible_paths = {
            filename,
            "../" + filename,
            "../../" + filename,
            "config/" + filename,
            "../config/" + filename,
            "../../config/" + filename
        };
        
        for (const auto& path : possible_paths) {
            std::ifstream test_file(path);
            if (test_file.is_open()) {
                test_file.close();
                return path;
            }
        }

        return possible_paths[0];
    }

    std::string findDocumentFile(const std::string& filename) {
        size_t last_slash = filename.find_last_of("/\\");
        std::string file_name = (last_slash != std::string::npos) ? filename.substr(last_slash + 1) : filename;
        std::string dir_name = (last_slash != std::string::npos) ? filename.substr(0, last_slash) : "";

        std::vector<std::string> possible_paths;
        
        if (!dir_name.empty()) {
            possible_paths = {
                filename,
                "../" + filename,
                "../../" + filename,
                dir_name + "/" + file_name,
                "../" + dir_name + "/" + file_name,
                "../../" + dir_name + "/" + file_name
            };
        } else {
            possible_paths = {
                file_name,
                "../" + file_name,
                "../../" + file_name
            };
        }
        
        for (const auto& path : possible_paths) {
            std::ifstream test_file(path);
            if (test_file.is_open()) {
                test_file.close();
                return path;
            }
        }

        return filename;
    }
    
    const std::string CONFIG_FILE_REL = "config/config.json";
    const std::string REQUESTS_FILE_REL = "config/request.json";
    const std::string ANSWERS_FILE = "answers.json";
    const std::string CONFIG_KEY = "config";
    const std::string FILES_KEY = "files";
    const std::string REQUESTS_KEY = "requests";
    const std::string MAX_RESPONSES_KEY = "max_responses";
    const std::string MAX_RESPONSE_KEY = "max_response";
    const std::string ANSWERS_KEY = "answers";
    const std::string RESULT_KEY = "result";
    const std::string RELEVANCE_KEY = "relevance";
    const std::string DOCID_KEY = "docid";
    const std::string RANK_KEY = "rank";
    const std::string REQUEST_PREFIX = "request";
    const std::string RESULT_TRUE = "true";
    const std::string RESULT_FALSE = "false";
    const std::string ERROR_CONFIG_MISSING = "config file is missing";
    const std::string ERROR_CONFIG_EMPTY = "config file is empty";
    const std::string ERROR_CANNOT_WRITE = "Cannot write to answers.json";
    const std::string WARNING_FILE_NOT_FOUND = "Warning: File not found: ";
    const int DEFAULT_MAX_RESPONSES = 5;
}

int ConverterJSON::GetResponseLimit() {
    std::string config_path = findConfigFile(CONFIG_FILE_REL);
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
        throw std::runtime_error(ERROR_CONFIG_MISSING);
    }
    
    json config = json::parse(config_file);
    config_file.close();

    if (!config.contains(CONFIG_KEY)) {
        throw std::runtime_error(ERROR_CONFIG_EMPTY);
    }

    if (config[CONFIG_KEY].contains(MAX_RESPONSES_KEY)) {
        return config[CONFIG_KEY][MAX_RESPONSES_KEY];
    } else if (config[CONFIG_KEY].contains(MAX_RESPONSE_KEY)) {
        return config[CONFIG_KEY][MAX_RESPONSE_KEY];
    } else {
        return DEFAULT_MAX_RESPONSES;
    }
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> documents;
    std::string config_path = findConfigFile(CONFIG_FILE_REL);
    std::ifstream config_file(config_path);
    
    if (!config_file.is_open()) {
        throw std::runtime_error(ERROR_CONFIG_MISSING);
    }
    
    json config = json::parse(config_file);
    config_file.close();
    
    if (!config.contains(CONFIG_KEY)) {
        throw std::runtime_error(ERROR_CONFIG_EMPTY);
    }

    if (!config.contains(FILES_KEY)) {
        return documents;
    }

    for (const auto& file_path : config[FILES_KEY]) {
        std::string original_path = file_path.get<std::string>();
        std::string path = findDocumentFile(original_path);
        std::ifstream file(path);
        
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            documents.push_back(buffer.str());
            file.close();
        } else {
            std::cerr << WARNING_FILE_NOT_FOUND << original_path << std::endl;
        }
    }
    
    return documents;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::vector<std::string> requests;
    std::string requests_path = findConfigFile(REQUESTS_FILE_REL);
    std::ifstream requests_file(requests_path);
    
    if (!requests_file.is_open()) {
        return requests;
    }
    
    json requests_json = json::parse(requests_file);
    requests_file.close();
    
    if (requests_json.contains(REQUESTS_KEY)) {
        for (const auto& request : requests_json[REQUESTS_KEY]) {
            requests.push_back(request.get<std::string>());
        }
    }
    
    return requests;
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    json result;
    json answers_obj;

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string request_id = REQUEST_PREFIX + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1);
        json request_result;
        
        if (answers[i].empty()) {
            request_result[RESULT_KEY] = RESULT_FALSE;
        } else {
            request_result[RESULT_KEY] = RESULT_TRUE;
            
            if (answers[i].size() > 1) {
                json relevance_array = json::array();
                for (const auto& pair : answers[i]) {
                    json doc_entry;
                    doc_entry[DOCID_KEY] = pair.first;
                    doc_entry[RANK_KEY] = pair.second;
                    relevance_array.push_back(doc_entry);
                }
                request_result[RELEVANCE_KEY] = relevance_array;
            } else {
                request_result[DOCID_KEY] = answers[i][0].first;
                request_result[RANK_KEY] = answers[i][0].second;
            }
        }
        
        answers_obj[request_id] = request_result;
    }
    
    result[ANSWERS_KEY] = answers_obj;

    std::ofstream answers_file(ANSWERS_FILE);
    if (answers_file.is_open()) {
        answers_file << result.dump(4);
        answers_file.close();
    } else {
        throw std::runtime_error(ERROR_CANNOT_WRITE);
    }
}