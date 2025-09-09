#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>

struct Entry {
    size_t doc_id;
    size_t count;

    bool operator ==(const Entry& other) const {
        return doc_id == other.doc_id && count == other.count;
    }
};

class InvertedIndex {
private:
    std::map<std::string, std::vector<Entry>> freq_dictionary;
    mutable std::mutex mutex;
    std::atomic<bool> is_indexing;

    void indexFile(const std::string& filePath, size_t docId);

public:
    InvertedIndex();

    // Запрещаем копирование и присваивание
    InvertedIndex(const InvertedIndex&) = delete;
    InvertedIndex& operator=(const InvertedIndex&) = delete;

    // Разрешаем перемещение
    InvertedIndex(InvertedIndex&& other) noexcept;
    InvertedIndex& operator=(InvertedIndex&& other) noexcept;

    void UpdateDocumentBase(const std::vector<std::string>& input_docs);
    std::vector<Entry> GetWordCount(const std::string& word) const;

    size_t GetNumDocuments() const;
    std::map<std::string, std::vector<Entry>> GetDictionary() const;
};