#include "converterJson.h"
#include <sstream>
#include <string>
#include <cmath>
#include <filesystem>

using namespace std;

bool ConverterJSON::fileConfigVerify() {
    filesystem::path configPath(fileConfig);
    if (!filesystem::exists(configPath)) {
        cout << "config file is missing" << endl;
        return false;
    }

    ifstream input(fileConfig);
    try {
        json config;
        input >> config;
        input.close();

        if (!config.contains("config") || config["config"].empty()) {
            cout << "config file is empty" << endl;
            return false;
        }

        string version = config["config"].value("version", "");
        if (version != "0.1") {
            cout << "config file has invalid version" << endl;
            return false;
        }

        cout << "name: " << config["config"].value("name", "") << "\n"
            << "version: " << version << endl;
        return true;
    }
    catch (const exception& e) {
        cout << "Error parsing config.json: " << e.what() << endl;
        return false;
    }
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    ifstream input(fileConfig);
    if (!input.is_open()) return {};

    try {
        json config;
        input >> config;
        input.close();

        vector<string> textDocuments;
        for (const auto& document : config["files"]) {
            filesystem::path docPath(document.get<string>());
            if (!filesystem::exists(docPath)) {
                cout << document.get<string>() << " file not found!" << endl;
                textDocuments.emplace_back();
                continue;
            }

            ifstream docInput(document.get<string>());
            string content((istreambuf_iterator<char>(docInput)),
                istreambuf_iterator<char>());
            textDocuments.push_back(content);
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
    ifstream input(fileConfig);
    if (!input.is_open()) return 5;

    try {
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

std::vector<std::string> ConverterJSON::GetRequests() {
    filesystem::path requestsPath(fileRequests);
    if (!filesystem::exists(requestsPath)) {
        cout << "request.json file is missing" << endl;
        return {};
    }

    ifstream input(fileRequests);
    if (!input.is_open()) return {};

    try {
        json request;
        input >> request;
        input.close();

        vector<string> requests = request.value("requests", vector<string>{});
        if (requests.size() > 1000) requests.resize(1000);
        return requests;
    }
    catch (const exception& e) {
        cout << "Error processing requests: " << e.what() << endl;
        return {};
    }
}

void ConverterJSON::putAnswers(const vector<vector<pair<int, float>>>& answers) {
    ofstream output(fileAnswers);
    if (!output.is_open()) {
        cout << "Cannot create answers.json!" << endl;
        return;
    }

    json result;
    int maxResponses = GetResponsesLimit();

    for (size_t i = 0; i < answers.size(); ++i) {
        string requestId = "request" + string(3 - to_string(i + 1).length(), '0') + to_string(i + 1);
        json answer;

        if (answers[i].empty()) {
            answer["result"] = false;
        }
        else {
            answer["result"] = true;
            json relevance = json::array();
            for (size_t j = 0; j < min(answers[i].size(), static_cast<size_t>(maxResponses)); ++j) {
                relevance.push_back({
                    {"docid", answers[i][j].first},
                    {"rank", round(answers[i][j].second * 1000) / 1000}
                    });
            }
            answer["relevance"] = relevance;
        }
        result["answers"][requestId] = answer;
    }

    output << result.dump(4);
    output.close();
    cout << "Results saved to answers.json" << endl;
}