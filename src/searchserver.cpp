#include "searchserver.h"
#include <sstream>
#include <algorithm>
#include <set>
#include <cmath> // для std::abs

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> result;

    for (const auto& query : queries_input) {
        std::istringstream stream(query);
        std::string word;
        std::set<std::string> unique_words;
        while (stream >> word) {
            word = _index.normalizeWord(word);
            if (!word.empty()) {
                unique_words.insert(word);
            }
        }

        if (unique_words.empty()) {
            result.push_back({});
            continue;
        }

        std::vector<std::string> words(unique_words.begin(), unique_words.end());
        std::sort(words.begin(), words.end(), [this](const std::string& a, const std::string& b) {
            size_t freq_a = 0;
            for (const auto& entry : _index.GetWordCount(a)) {
                freq_a += entry.count;
            }
            size_t freq_b = 0;
            for (const auto& entry : _index.GetWordCount(b)) {
                freq_b += entry.count;
            }
            return freq_a < freq_b;
        });

        std::set<size_t> doc_ids_set;
        for (const auto& w : words) {
            for (const auto& entry : _index.GetWordCount(w)) {
                doc_ids_set.insert(entry.doc_id);
            }
        }

        if (doc_ids_set.empty()) {
            result.push_back({});
            continue;
        }

        std::vector<RelativeIndex> relative_indexes;
        float max_rank = 0;
        for (auto doc_id : doc_ids_set) {
            float rank = 0;
            for (const auto& w : words) {
                for (const auto& entry : _index.GetWordCount(w)) {
                    if (entry.doc_id == doc_id) {
                        rank += entry.count;
                        break;
                    }
                }
            }
            if (rank > max_rank) {
                max_rank = rank;
            }
            relative_indexes.push_back({doc_id, rank});
        }

        for (auto& rel_index : relative_indexes) {
            rel_index.rank /= max_rank;
        }

        //сортировка
        std::sort(relative_indexes.begin(), relative_indexes.end(),
                  [](const RelativeIndex& a, const RelativeIndex& b) {
                      if (std::abs(a.rank - b.rank) > 0.0001f) {
                          return a.rank > b.rank;
                      }
                      return a.doc_id < b.doc_id;
                  });

        if (relative_indexes.size() > 5) {
            relative_indexes.resize(5);
        }

        result.push_back(relative_indexes);
    }

    return result;
}
