#include <QtTest>
#include <QCoreApplication>
#include <iostream>
#include <sstream>
#include "invertedindex.h"
#include "searchserver.h"

//Оператор вывода для RelativeIndex
inline std::ostream& operator<<(std::ostream& os, const RelativeIndex& ri) {
    os << "{doc_id: " << ri.doc_id << ", rank: " << ri.rank << "}";
    return os;
}

//Оператор вывода для вектора RelativeIndex (для отладки тестов)
inline std::ostream& operator<<(std::ostream& os, const std::vector<RelativeIndex>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i < vec.size() - 1) os << ", ";
    }
    os << "]";
    return os;
}

//вспомогательные функции для сравнения векторов RelativeIndex
bool compareRelativeIndexVectors(const std::vector<RelativeIndex>& actual,
                                 const std::vector<RelativeIndex>& expected) {
    if (actual.size() != expected.size()) {
        std::cout << "Different sizes: " << actual.size() << " vs " << expected.size() << std::endl;
        std::cout << "Actual: " << actual << std::endl;
        std::cout << "Expected: " << expected << std::endl;
        return false;
    }

    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i].doc_id != expected[i].doc_id) {
            std::cout << "Different doc_id at position " << i << ": "
                      << actual[i].doc_id << " vs " << expected[i].doc_id << std::endl;
            std::cout << "Actual: " << actual << std::endl;
            std::cout << "Expected: " << expected << std::endl;
            return false;
        }
        if (std::abs(actual[i].rank - expected[i].rank) > 0.001f) {
            std::cout << "Different rank at position " << i << ": "
                      << actual[i].rank << " vs " << expected[i].rank << std::endl;
            std::cout << "Actual: " << actual << std::endl;
            std::cout << "Expected: " << expected << std::endl;
            return false;
        }
    }
    return true;
}

void TestInvertedIndexFunctionality(
    const std::vector<std::string>& docs,
    const std::vector<std::string>& requests,
    const std::vector<std::vector<Entry>>& expected
    ) {
    std::vector<std::vector<Entry>> result;
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);

    for (auto& request : requests) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        result.push_back(word_count);
    }

    QVERIFY(result.size() == expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        QVERIFY(result[i].size() == expected[i].size());
        for (size_t j = 0; j < result[i].size(); ++j) {
            QVERIFY(result[i][j].doc_id == expected[i][j].doc_id);
            QVERIFY(result[i][j].count == expected[i][j].count);
        }
    }
}

class InvertedIndexTests : public QObject
{
    Q_OBJECT

private slots:
    void testBasicFunctionality();
    void testBasic2Functionality();
    void testMissingWordFunctionality();
    void testEnvironment();
};

class SearchServerTests : public QObject
{
    Q_OBJECT

private slots:
    void testSimple();
    void testTop5();
    void testEmptyQuery();
};

void InvertedIndexTests::testEnvironment()
{
    QVERIFY(1 == 1);
}

void InvertedIndexTests::testBasicFunctionality() {
    const std::vector<std::string> docs = {
        "london is the capital of great britain",
        "big ben is the nickname for the Great bell of the striking clock"
    };
    const std::vector<std::string> requests = {"london", "the"};
    const std::vector<std::vector<Entry>> expected = {
        { {0, 1} },
        { {0, 1}, {1, 3} }
    };

    TestInvertedIndexFunctionality(docs, requests, expected);
}

void InvertedIndexTests::testBasic2Functionality() {
    const std::vector<std::string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const std::vector<std::string> requests = {"milk", "water", "cappuccino"};
    const std::vector<std::vector<Entry>> expected = {
        { {0, 4}, {1, 1}, {2, 5} },
        { {0, 3}, {1, 2}, {2, 5} },
        { {3, 1} }
    };

    TestInvertedIndexFunctionality(docs, requests, expected);
}

void InvertedIndexTests::testMissingWordFunctionality() {
    const std::vector<std::string> docs = {
        "a b c d e f g h i j k l",
        "statement"
    };
    const std::vector<std::string> requests = {"m", "statement"};
    const std::vector<std::vector<Entry>> expected = {
        {},
        { {1, 1} }
    };

    TestInvertedIndexFunctionality(docs, requests, expected);
}

void SearchServerTests::testSimple() {
    const std::vector<std::string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const std::vector<std::string> request = {"milk water", "sugar"};
    const std::vector<std::vector<RelativeIndex>> expected = {
        {
            {2, 1.0f},
            {0, 0.7f},
            {1, 0.3f}
        },
        {
        }
    };

    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(request);

    QVERIFY(result.size() == expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        QVERIFY2(compareRelativeIndexVectors(result[i], expected[i]),
                 "Vectors are not equal");
    }
}

void SearchServerTests::testTop5() {
    const std::vector<std::string> docs = {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo is the capital of norway",
        "stockholm is the capital of sweden",
        "riga is the capital of latvia",
        "tallinn is the capital of estonia",
        "warsaw is the capital of poland",
    };
    const std::vector<std::string> request = {"moscow is the capital of russia"};
    const std::vector<std::vector<RelativeIndex>> expected = {
        {
            {7, 1.0f},
            {14, 1.0f},
            {0, 0.666666687f},
            {1, 0.666666687f},
            {2, 0.666666687f}
        }
    };

    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(request);

    QVERIFY(result.size() == expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        QVERIFY2(compareRelativeIndexVectors(result[i], expected[i]),
                 "Vectors are not equal");
    }
}

void SearchServerTests::testEmptyQuery() {
    const std::vector<std::string> docs = {
        "some text here",
        "another text there"
    };
    const std::vector<std::string> request = {""};

    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(request);

    QVERIFY(result.size() == 1);
    QVERIFY(result[0].empty());
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    InvertedIndexTests test1;
    int result1 = QTest::qExec(&test1, argc, argv);

    SearchServerTests test2;
    int result2 = QTest::qExec(&test2, argc, argv);

    return result1 || result2;
}

#include "test_main.moc"
