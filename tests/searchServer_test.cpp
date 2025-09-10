#include "searchServer.h"
#include "gtest/gtest.h"
using namespace std;

TEST(TestCaseSearchServer, TestSimple) {
    const vector<string> docs = {
            "milk milk milk milk water water water",
            "milk water water",
            "milk milk milk milk milk water water water water water",
            "americano cappuccino"
    };
    const vector<string> request = { "milk water", "sugar" };
    const std::vector<vector<RelativeIndex>> expected = {
            {{2, 1.0f}, {0, 0.7f}, {1, 0.3f}},
            {}
    };
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(std::move(idx), 5);
    std::vector<vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result, expected);
}

TEST(TestCaseSearchServer, TestTop5) {
    const vector<string> docs = {
            "london is the capital of great britain",
            "paris is the capital of france",
            "berlin is the capital of germany",
            "rome is the capital of italy",
            "madrid is the capital of spain",
            "lisboa is the capital of portugal",
            "bern is the capital of switzerland",
            "moscow is the capital of russia",                               // 7
            "kiev is the capital of ukraine",
            "minsk is the capital of belarus",
            "astana is the capital of kazakhstan",
            "beijing is the capital of china",
            "tokyo is the capital of japan",
            "bangkok is the capital of thailand",
            "welcome to moscow the capital of russia the third rome",         // 14
            "amsterdam is the capital of netherlands",
            "helsinki is the capital of finland",
            "oslo is the capital of norway",
            "stockholm is the capital of sweden",
            "riga is the capital of latvia",
            "tallinn is the capital of estonia",
            "warsaw is the capital of poland",
    };
    const vector<string> request = { "moscow is the capital of russia" };
    const std::vector<vector<RelativeIndex>> expected = {
            {
                    {7, 1.0f}
            }
    };
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(std::move(idx), 5);
    std::vector<vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result, expected);
}

