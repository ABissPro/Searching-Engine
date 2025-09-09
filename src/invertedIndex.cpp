#include "invertedIndex.h"
#include <sstream>
#include <thread>
#include <algorithm>
#include <cctype>
#include <map>

static std::string normalize_token(const std::string& token) {
    std::string out;
    out.reserve(token.size());
    for (unsigned char c : token) {
        if (isalpha(c)) out.push_back(static_cast<char>(tolower(c)));
        // игнорируем прочие символы
    }
    return out;
}

InvertedIndex::InvertedIndex() : is_indexing(false), documentsCount(0) {}

InvertedIndex::InvertedIndex(InvertedIndex&& other) noexcept
    : freq_dictionary(std::move(other.freq_dictionary)),
    is_indexing(other.is_indexing.load()),
    documentsCount(other.documentsCount) {
}

InvertedIndex& InvertedIndex::operator=(InvertedIndex&& other) noexcept {
    if (this != &other) {
        freq_dictionary = std::move(other.freq_dictionary);
        is_indexing = other.is_indexing.load();
        documentsCount = other.documentsCount;
    }
    return *this;
}

void InvertedIndex::indexFile(const std::string& documentText, size_t docId) {
    std::istringstream ss(documentText);
    std::string token;
    std::map<std::string, size_t> wordCounts;

    while (ss >> token) {
        std::string w = normalize_token(token);
        if (w.empty()) continue;
        ++wordCounts[w];
    }

    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& [w, count] : wordCounts) {
        freq_dictionary[w].push_back({ docId, count });
    }
}

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    bool expected = false;
    // не позволяем параллельный вызов
    if (!is_indexing.compare_exchange_strong(expected, true)) return;

    {
        std::lock_guard<std::mutex> lock(mutex);
        freq_dictionary.clear();
        documentsCount = input_docs.size();
    }

    std::vector<std::thread> threads;
    threads.reserve(input_docs.size());
    for (size_t i = 0; i < input_docs.size(); ++i) {
        threads.emplace_back(&InvertedIndex::indexFile, this, input_docs[i], i);
    }

    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }

    // сортируем векторы Entry по doc_id
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& [word, entries] : freq_dictionary) {
            std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
                return a.doc_id < b.doc_id;
                });
        }
    }

    is_indexing.store(false);
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = freq_dictionary.find(word);
    if (it != freq_dictionary.end()) return it->second;
    return {};
}

size_t InvertedIndex::GetNumDocuments() const {
    return documentsCount;
}

std::map<std::string, std::vector<Entry>> InvertedIndex::GetDictionary() const {
    std::lock_guard<std::mutex> lock(mutex);
    return freq_dictionary;
}
