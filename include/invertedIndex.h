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
    size_t documentsCount; // количество документов в индексе

    // “еперь метод принимает не путь к файлу, а содержимое документа (строку)
    void indexFile(const std::string& documentText, size_t docId);

public:
    InvertedIndex();

    // «апрещаем копирование и присваивание
    InvertedIndex(const InvertedIndex&) = delete;
    InvertedIndex& operator=(const InvertedIndex&) = delete;

    // –азрешаем перемещение
    InvertedIndex(InvertedIndex&& other) noexcept;
    InvertedIndex& operator=(InvertedIndex&& other) noexcept;

    void UpdateDocumentBase(const std::vector<std::string>& input_docs);
    std::vector<Entry> GetWordCount(const std::string& word) const;

    size_t GetNumDocuments() const;
    std::map<std::string, std::vector<Entry>> GetDictionary() const;
};
