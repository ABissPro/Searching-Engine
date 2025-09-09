#include "converterJson.h"
#include "gtest/gtest.h"
#include <fstream>

TEST(ConverterJSONTest, ConfigValidation) {
    // Тест проверки конфигурации
    EXPECT_TRUE(ConverterJSON::fileConfigVerify());
}

TEST(ConverterJSONTest, GetResponsesLimit) {
    // Тест получения лимита ответов
    int limit = ConverterJSON::GetResponsesLimit();
    EXPECT_GE(limit, 1);
    EXPECT_LE(limit, 100);
}

TEST(ConverterJSONTest, GetTextDocuments) {
    // Тест получения текстов документов
    auto docs = ConverterJSON::GetTextDocuments();
    EXPECT_FALSE(docs.empty());
}

TEST(ConverterJSONTest, GetRequests) {
    // Тест получения запросов
    auto request = ConverterJSON::GetRequests();
    EXPECT_FALSE(request.empty());
}