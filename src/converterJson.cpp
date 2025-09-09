#include "converterJson.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>  // Перемещено в начало для setw/setfill

using namespace std;
namespace fs = std::filesystem;

fs::path ConverterJSON::findFileUpwards(const std::string& filename, int maxDepth) {
    fs::path cur = fs::current_path();
    for (int i = 0; i <= maxDepth; ++i) {
        fs::path candidate = cur / filename;
        if (fs::exists(candidate)) return candidate;
        cur = cur.parent_path();
    }
    // last attempt: executable directory (on some setups)
    fs::path exeDir = fs::current_path();
    fs::path alt = exeDir / filename;
    if (fs::exists(alt)) return alt;
    return {};
}

string ConverterJSON::normalizeWord(const string& s) {
    string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        if (isalpha(c)) out.push_back(static_cast<char>(tolower(c)));
        // else ignore punctuation/digits
    }
    return out;
}

bool ConverterJSON::fileConfigVerify() {
    auto configPath = findFileUpwards(fileConfig);
    if (configPath.empty()) {
        cout << "config file is missing" << endl;
        return false;
    }

    try {
        ifstream input(configPath);
        if (!input.is_open()) {
            cout << "config file is missing" << endl;
            return false;
        }
        json config;
        input >> config;
        input.close();

        if (!config.contains("config") ||
            !config["config"].contains("name") ||
            !config["config"].contains("version")) {
            cout << "config file is empty" << endl;
            return false;
        }

        string version = config["config"]["version"].get<string>();
        if (version != "0.1") {
            cout << "config.json has incorrect file version" << endl;  // Исправлено на точное сообщение из ТЗ
            return false;
        }

        // Исправлено: вывод "Starting <name>" как в ТЗ
        cout << "Starting " << config["config"]["name"] << endl;
        cout << "version: " << version << endl;
        return true;
    }
    catch (const exception& e) {
        cout << "Error parsing config.json: " << e.what() << endl;
        return false;
    }
}

vector<string> ConverterJSON::GetTextDocuments() {
    auto configPath = findFileUpwards(fileConfig);
    if (configPath.empty()) return {};

    try {
        ifstream input(configPath);
        if (!input.is_open()) return {};
        json config;
        input >> config;
        input.close();

        vector<string> textDocuments;
        fs::path base = configPath.parent_path();

        for (const auto& document : config["files"]) {
            string docPathStr = document.get<string>();
            fs::path docPath(docPathStr);
            if (docPath.is_relative()) docPath = base / docPath;

            ifstream docInput(docPath);
            if (!docInput.is_open()) {
                cout << docPath.string() << " file not found!" << endl;
                textDocuments.emplace_back(); // пустой документ — тесты учитывают это
                continue;
            }

            string token;
            string words;
            int countWords = 0;
            while (docInput >> token && countWords < 1000) {
                string norm = normalizeWord(token);
                if (norm.empty()) continue;
                if (!words.empty()) words += ' ';
                // ensure word length limit:
                if (norm.size() > 100) norm = norm.substr(0, 100);
                words += norm;
                ++countWords;
            }
            textDocuments.push_back(words);
            docInput.close();
        }
        return textDocuments;
    }
    catch (const exception& e) {
        cout << "Error processing documents: " << e.what() << endl;
        return {};
    }
}

int ConverterJSON::GetResponsesLimit() {
    auto configPath = findFileUpwards(fileConfig);
    if (configPath.empty()) return 5;

    try {
        ifstream input(configPath);
        if (!input.is_open()) return 5;
        json config;
        input >> config;
        input.close();
        return config["config"].value("max_responses", 5);
    }
    catch (const exception& e) {
        cout << "Error reading max_responses: " << e.what() << endl;
        return 5;
    }
}

vector<string> ConverterJSON::GetRequests() {
    auto reqPath = findFileUpwards(fileRequests);
    if (reqPath.empty()) return {};

    try {
        ifstream input(reqPath);
        if (!input.is_open()) return {};
        json request;
        input >> request;
        input.close();

        vector<string> tempValue = request["requests"].get<vector<string>>();
        if (tempValue.size() > 1000) tempValue.resize(1000);

        vector<string> normalized;
        normalized.reserve(tempValue.size());
        for (const auto& item : tempValue) {
            // normalize whole query: split, normalize words, then rejoin
            stringstream ss(item);
            string token;
            int wordCount = 0;
            string rebuilt;
            while (ss >> token && wordCount < 10) {
                string norm = normalizeWord(token);
                if (norm.empty()) continue;
                if (!rebuilt.empty()) rebuilt += ' ';
                rebuilt += norm;
                ++wordCount;
            }
            if (!rebuilt.empty()) normalized.push_back(rebuilt);
            else normalized.push_back(string()); // keep same count/order as input
        }
        return normalized;
    }
    catch (const exception& e) {
        cout << "Error processing requests: " << e.what() << endl;
        return {};
    }
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    auto cfg = findFileUpwards(fileConfig);
    fs::path outPath;
    if (!cfg.empty()) outPath = cfg.parent_path() / fileAnswers;
    else outPath = fs::current_path() / fileAnswers;

    std::ofstream output(outPath);
    if (!output.is_open()) {
        std::cerr << "Cannot create " << outPath.string() << std::endl;
        return;
    }

    json result;
    int maxResponses = GetResponsesLimit();

    for (size_t i = 0; i < answers.size(); ++i) {
        std::ostringstream idx;
        idx << std::setw(3) << std::setfill('0') << (i + 1);
        std::string requestId = "request" + idx.str();

        json answer;
        if (answers[i].empty()) {
            answer["result"] = "false";
        }
        else {
            answer["result"] = "true";
            if (answers[i].size() == 1) {
                answer["docid"] = answers[i][0].first;
                answer["rank"] = std::round(answers[i][0].second * 1000.0f) / 1000.0f;
            }
            else {
                json relevance = json::array();
                size_t limit = std::min(answers[i].size(), static_cast<size_t>(maxResponses));
                for (size_t j = 0; j < limit; ++j) {
                    relevance.push_back({
                        {"docid", answers[i][j].first},
                        {"rank", std::round(answers[i][j].second * 1000.0f) / 1000.0f}
                        });
                }
                answer["relevance"] = relevance;
            }
        }
        result["answers"][requestId] = answer;
    }

    output << result.dump(4);
    output.close();
    std::cout << "Results saved to " << outPath.string() << std::endl;
}
