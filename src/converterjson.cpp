#include "converterjson.h"
#include "searchserver.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

//Конструктор
ConverterJSON::ConverterJSON() : configPath("config.json") {}

//путь к конфигурационному файлу
void ConverterJSON::setConfigPath(const std::string& path) {
    configPath = path;
}

//Получение текстов документов
std::vector<std::string> ConverterJSON::getTextDocuments() const {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Config file not found: " << configPath << std::endl;
        return {};
    }

    json config;
    file >> config;
    file.close();

    std::vector<std::string> documents;
    for (const auto& filePath : config["files"]) {
        std::ifstream docFile(filePath);
        if (!docFile.is_open()) {
            std::cerr << "Document file not found: " << filePath << std::endl;
            continue;
        }
        std::string content((std::istreambuf_iterator<char>(docFile)),
                            std::istreambuf_iterator<char>());
        documents.push_back(content);
        docFile.close();
    }
    return documents;
}

//Получение запросов
std::vector<std::string> ConverterJSON::getRequests() const {
    std::ifstream file("requests.json");
    if (!file.is_open()) {
        std::cerr << "Requests file not found" << std::endl;
        return {};
    }

    json requests;
    file >> requests;
    file.close();

    return requests["requests"];
}

//Получение ограничения на количество ответов
int ConverterJSON::getResponsesLimit() const {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Config file not found" << std::endl;
        return 5;
    }

    json config;
    file >> config;
    file.close();

    return config["config"]["max_responses"];
}

//Сохранение ответов
void ConverterJSON::putAnswers(const std::vector<std::vector<RelativeIndex>>& answers) const {
    json j;
    j["answers"] = json::object();

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string requestName = "request" + std::to_string(i);
        if (answers[i].empty()) {
            j["answers"][requestName] = {{"result", false}};
        } else {
            j["answers"][requestName] = {{"result", true}};
            j["answers"][requestName]["relevance"] = json::array();
            for (const auto& relIndex : answers[i]) {
                j["answers"][requestName]["relevance"].push_back({
                    {"docid", relIndex.doc_id},
                    {"rank", relIndex.rank}
                });
            }
        }
    }

    std::ofstream file("answers.json");
    file << j.dump(4);
    file.close();
}
