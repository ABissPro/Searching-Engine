#include "searchServer.h"
#include <sstream>
#include <set>
#include <algorithm>
#include <iostream>

using namespace std;

vector<vector<RelativeIndex>> SearchServer::search(const vector<std::string>& queries_input) {
    vector<vector<RelativeIndex>> results(queries_input.size());
    vector<thread> threads;

    auto processQuery = [this](const string& query, size_t queryIndex, vector<RelativeIndex>& result) {
        istringstream ss(query);
        set<string> unique_words;
        string word;

        while (ss >> word) {
            // Convert to lowercase for consistency
            transform(word.begin(), word.end(), word.begin(),
                [](unsigned char c) { return tolower(c); });
            unique_words.insert(word);
        }

        map<size_t, float> absRelevance;
        float maxRelevance = 0;

        for (const auto& word : unique_words) {
            auto entries = index.GetWordCount(word);
            for (const auto& entry : entries) {
                absRelevance[entry.doc_id] += entry.count;
                maxRelevance = max(maxRelevance, absRelevance[entry.doc_id]);
            }
        }

        if (maxRelevance > 0) {
            for (const auto& [docId, relevance] : absRelevance) {
                result.push_back({ docId, relevance / maxRelevance });
            }
        }

        sort(result.begin(), result.end(), [](const RelativeIndex& a, const RelativeIndex& b) {
            return a.rank > b.rank || (a.rank == b.rank && a.doc_id < b.doc_id);
            });

        if (result.size() > responsesLimit) {
            result.resize(responsesLimit);
        }
        };

    for (size_t i = 0; i < queries_input.size(); ++i) {
        threads.emplace_back(processQuery, queries_input[i], i, ref(results[i]));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return results;
}