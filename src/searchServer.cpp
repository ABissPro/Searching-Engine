#include "searchServer.h"
#include <sstream>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>

using namespace std;

static vector<string> split_and_normalize(const string& query) {
    stringstream ss(query);
    string token;
    vector<string> words;
    while (ss >> token) {
        string norm;
        for (unsigned char c : token) {
            if (isalpha(c)) norm.push_back(static_cast<char>(tolower(c)));
        }
        if (!norm.empty()) words.push_back(norm);
    }
    return words;
}

vector<vector<RelativeIndex>> SearchServer::search(const vector<string>& queries_input) {
    // –езультирующий вектор фиксированного размера Ч чтобы вернуть ответы в пор€дке запросов
    vector<vector<RelativeIndex>> results(queries_input.size());

    vector<thread> threads;
    threads.reserve(queries_input.size());
    mutex resultsMutex;

    // ”далЄн broken-блок: только корректные потоки
    for (size_t qi = 0; qi < queries_input.size(); ++qi) {
        threads.emplace_back([this, &results, &resultsMutex, qi, &queries_input]() {  // явный захват this, results, mutex по ref; qi по value; queries_input по ref
            const string& query = queries_input[qi];
            vector<string> words = split_and_normalize(query);
            if (words.empty()) {
                lock_guard<mutex> g(resultsMutex);
                results[qi] = {};
                return;
            }

            // ”никализируем слова (пор€док слов при пересечении не важен)
            unordered_set<string> uniqSet(words.begin(), words.end());
            vector<string> uniqWords(uniqSet.begin(), uniqSet.end());

            // ƒл€ AND: начнЄм с множества документов первого слова, затем пересекаем
            // ѕолучим вхождени€ первого слова
            auto firstEntries = index.GetWordCount(uniqWords[0]);
            if (firstEntries.empty()) {
                lock_guard<mutex> g(resultsMutex);
                results[qi] = {}; // пересечение пусто
                return;
            }

            // »спользуем map дл€ хранени€ суммарного количества вхождений (абсолютной релевантности)
            unordered_map<size_t, size_t> absRelevance;
            unordered_set<size_t> currentDocs;
            for (const auto& e : firstEntries) {
                currentDocs.insert(e.doc_id);
                absRelevance[e.doc_id] = e.count;
            }

            // ƒл€ каждого следующего слова Ч вычисл€ем множество документов и пересекаем
            for (size_t wi = 1; wi < uniqWords.size(); ++wi) {
                const string& w = uniqWords[wi];
                auto entries = index.GetWordCount(w);
                if (entries.empty()) {
                    currentDocs.clear();
                    break;
                }
                unordered_map<size_t, size_t> countsThisWord;
                for (const auto& e : entries) countsThisWord[e.doc_id] = e.count;

                vector<size_t> toRemove;
                for (auto docid : currentDocs) {
                    auto it = countsThisWord.find(docid);
                    if (it == countsThisWord.end()) {
                        toRemove.push_back(docid);
                    }
                    else {
                        absRelevance[docid] += it->second;
                    }
                }
                for (auto d : toRemove) currentDocs.erase(d);
                if (currentDocs.empty()) break;
            }

            vector<RelativeIndex> rels;
            if (!currentDocs.empty()) {
                // ЌайдЄм максимум и нормализуем
                size_t maxAbs = 0;
                for (auto docid : currentDocs) {
                    if (absRelevance[docid] > maxAbs) maxAbs = absRelevance[docid];
                }
                for (auto docid : currentDocs) {
                    float rank = (maxAbs > 0) ? static_cast<float>(absRelevance[docid]) / static_cast<float>(maxAbs) : 0.0f;
                    rels.push_back({ docid, rank });
                }

                sort(rels.begin(), rels.end(), [](const RelativeIndex& a, const RelativeIndex& b) {
                    if (a.rank != b.rank) return a.rank > b.rank;
                    return a.doc_id < b.doc_id;
                    });

                if (static_cast<int>(rels.size()) > responsesLimit) rels.resize(responsesLimit);
            }

            // записываем именно в слот с индексом qi (сохранение пор€дка)
            {
                lock_guard<mutex> g(resultsMutex);
                results[qi] = std::move(rels);
            }
            });
    }

    for (auto& t : threads) if (t.joinable()) t.join();

    return results;
}
