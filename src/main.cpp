#include "converterJson.h"
#include "invertedIndex.h"
#include "searchServer.h"

using namespace std;

int main() {
    if (!ConverterJSON::fileConfigVerify()) {
        return 1;
    }

    try {
        InvertedIndex idx;
        auto docs = ConverterJSON::GetTextDocuments();
        if (docs.empty()) {
            throw runtime_error("No documents to index");
        }
        idx.UpdateDocumentBase(docs);

        auto requests = ConverterJSON::GetRequests();
        if (requests.empty()) {
            throw runtime_error("No requests to process");
        }

        SearchServer server(idx, ConverterJSON::GetResponsesLimit());
        auto answers = server.search(requests);

        vector<vector<pair<int, float>>> convertedAnswers;
        for (const auto& queryResults : answers) {
            convertedAnswers.emplace_back();
            for (const auto& result : queryResults) {
                convertedAnswers.back().emplace_back(result.doc_id, result.rank);
            }
        }

        ConverterJSON::putAnswers(convertedAnswers);
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}