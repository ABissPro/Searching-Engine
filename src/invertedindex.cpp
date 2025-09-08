#include "invertedindex.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <thread>

std::string InvertedIndex::normalizeWord(const std::string& word) const {
    std::string result;
    std::remove_copy_if(word.begin(), word.end(),
                        std::back_inserter(result),
                        [](char c) { return std::ispunct(c); });
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    docs = input_docs;
    freq_dictionary.clear();
    wordsCountPerDoc.resize(docs.size(), 0);

    std::vector<std::thread> threads;

    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        threads.emplace_back([this, doc_id]() {
            std::istringstream stream(docs[doc_id]);
            std::string word;
            size_t wordCount = 0;

            std::map<std::string, size_t> localDict;

            while (stream >> word) {
                word = normalizeWord(word);
                if (word.empty()) continue;

                wordCount++;
                localDict[word]++;
            }

            wordsCountPerDoc[doc_id] = wordCount;

            std::lock_guard<std::mutex> lock(dict_mutex);

            for (const auto& [word, count] : localDict) {
                if (freq_dictionary.find(word) != freq_dictionary.end()) {
                    bool found = false;
                    for (auto& entry : freq_dictionary[word]) {
                        if (entry.doc_id == doc_id) {
                            entry.count += count;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        freq_dictionary[word].push_back({doc_id, count});
                    }
                } else {
                    freq_dictionary[word] = {{doc_id, count}};
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {  // Добавлен const
    std::string normalizedWord = normalizeWord(word);

    std::lock_guard<std::mutex> lock(dict_mutex);

    auto it = freq_dictionary.find(normalizedWord);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}

size_t InvertedIndex::getWordsCountInDoc(size_t doc_id) const {
    if (doc_id < wordsCountPerDoc.size()) {
        return wordsCountPerDoc[doc_id];
    }
    return 0;
}
