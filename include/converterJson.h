#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>

constexpr char fileConfig[] = "config.json";
constexpr char fileRequests[] = "requests.json";  // ����������: ��������� 's' ��� ������������ ��
constexpr char fileAnswers[] = "answers.json";

using json = nlohmann::json;

class ConverterJSON {
public:
    ConverterJSON() = default;

    // ��������� ������� � ������������ config.json
    static bool fileConfigVerify();

    // ���������� ���������� ����������, ������������� � config.json
    static std::vector<std::string> GetTextDocuments();

    // ��������� max_responses (���� ����������� � 5)
    static int GetResponsesLimit();

    // ��������� ������ �������� �� requests.json
    static std::vector<std::string> GetRequests();

    // ���������� answers � answers.json
    static void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    // ������� ����� ���� � ������ ��������������� ������������ CWD
    static std::filesystem::path findFileUpwards(const std::string& filename, int maxDepth = 5);

    // ����������� �����: ��������� ������ ��������� �����, ��������� � ������ �������
    static std::string normalizeWord(const std::string& s);
};
