#include "converterJson.h"
#include "invertedIndex.h"
#include "searchServer.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <utility>
#include <iomanip>

namespace fs = std::filesystem;
using namespace std;

int main() {
    try {
        if (!ConverterJSON::fileConfigVerify()) {
            cerr << "Config verification failed. Aborting." << endl;
            return 1;
        }

        auto docs = ConverterJSON::GetTextDocuments();
        auto requests = ConverterJSON::GetRequests();
        int maxResponses = ConverterJSON::GetResponsesLimit();

        cout << "Docs loaded: " << docs.size() << ", Requests: " << requests.size() << ", maxResponses: " << maxResponses << endl;
        if (requests.empty()) {
            cout << "Warning: No requests loaded. Creating empty answers.json." << endl;
        }
        if (docs.empty()) {
            cout << "Warning: No documents loaded. Search will return false for all." << endl;
        }

        InvertedIndex idx;
        idx.UpdateDocumentBase(docs);

        SearchServer server(std::move(idx), maxResponses);

        auto raw = server.search(requests);

        vector<vector<pair<int, float>>> answers;
        answers.reserve(raw.size());
        for (const auto& block : raw) {
            vector<pair<int, float>> b;
            b.reserve(block.size());
            for (const auto& ri : block) {
                b.emplace_back(static_cast<int>(ri.doc_id), ri.rank);
            }
            answers.push_back(std::move(b));
        }

        cout << "Current working directory: " << fs::current_path().string() << endl;

        ConverterJSON::putAnswers(answers);

        cout << "Main finished normally." << endl;
        return 0;
    }
    catch (const std::exception& e) {
        cerr << "Unhandled exception in main: " << e.what() << endl;
        return 3;
    }
    catch (...) {
        cerr << "Unhandled unknown exception in main." << endl;
        return 3;
    }
}

