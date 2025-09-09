#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>

constexpr char fileConfig[] = "config.json";
constexpr char fileRequests[] = "requests.json";  // Исправлено: добавлена 's' для соответствия ТЗ
constexpr char fileAnswers[] = "answers.json";

using json = nlohmann::json;

class ConverterJSON {
public:
    ConverterJSON() = default;

    // Проверяет наличие и корректность config.json
    static bool fileConfigVerify();

    // Возвращает содержимое документов, перечисленных в config.json
    static std::vector<std::string> GetTextDocuments();

    // Считывает max_responses (если отсутствует — 5)
    static int GetResponsesLimit();

    // Считывает список запросов из requests.json
    static std::vector<std::string> GetRequests();

    // Записывает answers в answers.json
    static void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    // Попытки найти файл в разных местоположениях относительно CWD
    static std::filesystem::path findFileUpwards(const std::string& filename, int maxDepth = 5);

    // Нормализует слово: оставляет только латинские буквы, переводит в нижний регистр
    static std::string normalizeWord(const std::string& s);
};
