#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <thread>
#include <mutex>
#include "invertedIndex.h"
#include "converterJSON.h"

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

class SearchServer {
public:
    explicit SearchServer(InvertedIndex idx, int responsesLimit = 5) :
        index(std::move(idx)), responsesLimit(responsesLimit) {
    };

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex index;
    int responsesLimit;
};