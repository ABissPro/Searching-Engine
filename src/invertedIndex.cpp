#include "invertedIndex.h"
#include <sstream>
#include <thread>
#include <algorithm>
#include <iostream>

InvertedIndex::InvertedIndex() : is_indexing(false) {}

InvertedIndex::InvertedIndex(InvertedIndex&& other) noexcept
    : freq_dictionary(std::move(other.freq_dictionary)),
    is_indexing(other.is_indexing.load()) {
}

InvertedIndex& InvertedIndex::operator=(InvertedIndex&& other) noexcept {
    if (this != &other) {
        freq_dictionary = std::move(other.freq_dictionary);
        is_indexing = other.is_indexing.load();
    }
    return *this;
}

void InvertedIndex::indexContent(const std::string& content, size_t docId) {
    std::istringstream stream(content);
    std::string word;
    std::map<std::string, size_t> wordCounts;

    while (stream >> word) {
        // Convert to lowercase for consistency
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c) { return std::tolower(c); });
        ++wordCounts[word];
    }

    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& [word, count] : wordCounts) {
        freq_dictionary[word].push_back({ docId, count });
    }
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    if (is_indexing) return;
    is_indexing = true;

    freq_dictionary.clear();
    std::vector<std::thread> threads;
    for (size_t i = 0; i < input_docs.size(); ++i) {
        threads.emplace_back(&InvertedIndex::indexContent, this, input_docs[i], i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Sort entries by doc_id for each word
    for (auto& [word, entries] : freq_dictionary) {
        std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
            return a.doc_id < b.doc_id;
            });
    }

    is_indexing = false;
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(),
        [](unsigned char c) { return std::tolower(c); });

    std::lock_guard<std::mutex> lock(mutex);
    auto it = freq_dictionary.find(lowerWord);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}

size_t InvertedIndex::GetNumDocuments() const {
    // This would need to be implemented based on your logic
    return 0;
}

std::map<std::string, std::vector<Entry>> InvertedIndex::GetDictionary() const {
    std::lock_guard<std::mutex> lock(mutex);
    return freq_dictionary;
}