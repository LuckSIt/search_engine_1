#include "../include/SearchServer.h"
#include "../include/InvertedIndex.h"
#include "gtest/gtest.h"

using namespace std;

/**
 * Этап 5: Тесты для SearchServer
 * 
 * Эти тесты проверяют корректность работы поискового сервера:
 * - Правильность расчета релевантности
 * - Корректность сортировки результатов
 * - Обработку запросов без результатов
 */

TEST(TestCaseSearchServer, TestSimple) {
    const vector<string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const vector<string> request = {
        "milk water",
        "sugar"
    };
    const std::vector<vector<RelativeIndex>> expected = {
        {
            {2, 1.0f},
            {0, 0.7f},
            {1, 0.3f}
        },
        {}
    };
    
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<vector<RelativeIndex>> result = srv.search(request);
    
    ASSERT_EQ(result.size(), expected.size());
    
    for (size_t i = 0; i < result.size(); ++i) {
        ASSERT_EQ(result[i].size(), expected[i].size());
        for (size_t j = 0; j < result[i].size(); ++j) {
            EXPECT_EQ(result[i][j].doc_id, expected[i][j].doc_id);
            EXPECT_NEAR(result[i][j].rank, expected[i][j].rank, 0.01f);
        }
    }
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
        "warsaw is the capital of poland"
    };
    const vector<string> request = {
        "moscow is the capital of russia"
    };
    const std::vector<vector<RelativeIndex>> expected = {
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
    std::vector<vector<RelativeIndex>> result = srv.search(request);
    
    ASSERT_EQ(result.size(), expected.size());
    ASSERT_EQ(result[0].size(), expected[0].size());
    
    for (size_t i = 0; i < result[0].size(); ++i) {
        EXPECT_EQ(result[0][i].doc_id, expected[0][i].doc_id);
        EXPECT_NEAR(result[0][i].rank, expected[0][i].rank, 0.01f);
    }
}
