#include "converterJson.h"
#include "gtest/gtest.h"
#include <fstream>

TEST(ConverterJSONTest, ConfigValidation) {
    EXPECT_TRUE(ConverterJSON::fileConfigVerify());
}

TEST(ConverterJSONTest, GetResponsesLimit) {
    int limit = ConverterJSON::GetResponsesLimit();
    EXPECT_GE(limit, 1);
    EXPECT_LE(limit, 100);
}

TEST(ConverterJSONTest, GetTextDocuments) {
    auto docs = ConverterJSON::GetTextDocuments();
    EXPECT_FALSE(docs.empty());
}

TEST(ConverterJSONTest, GetRequests) {
    auto request = ConverterJSON::GetRequests();
    EXPECT_FALSE(request.empty());

}
