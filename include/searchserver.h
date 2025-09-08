#ifndef SEARCHSERVER_H
#define SEARCHSERVER_H

#include "invertedindex.h"

struct RelativeIndex {
    size_t doc_id;
    float rank;
    bool operator==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && std::abs(rank - other.rank) < 0.001f);
    }
};

class SearchServer {
public:
    SearchServer(InvertedIndex& idx) : _index(idx) { }
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& _index;
};

#endif
