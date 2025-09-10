#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>

constexpr char fileConfig[] = "config.json";
constexpr char fileRequests[] = "requests.json"; 
constexpr char fileAnswers[] = "answers.json";

using json = nlohmann::json;

class ConverterJSON {
public:
    ConverterJSON() = default;

    static bool fileConfigVerify();

    static std::vector<std::string> GetTextDocuments();

    static int GetResponsesLimit();

    static std::vector<std::string> GetRequests();

    static void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    static std::filesystem::path findFileUpwards(const std::string& filename, int maxDepth = 5);

    static std::string normalizeWord(const std::string& s);
};

