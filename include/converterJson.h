#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

constexpr char fileConfig[] = "./config.json";
constexpr char fileRequests[] = "./request.json";
constexpr char fileAnswers[] = "./answers.json";

using json = nlohmann::json;

class ConverterJSON {
public:
    ConverterJSON() = default;

    static bool fileConfigVerify();
    static std::vector<std::string> GetTextDocuments();
    static int GetResponsesLimit();
    static std::vector<std::string> GetRequests();
    static void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);
};