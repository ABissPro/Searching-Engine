#include "converterJson.h"
#include "invertedIndex.h"
#include "searchServer.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <utility>
#include <iomanip>

namespace fs = std::filesystem;
using namespace std;

int main() {
    try {
        // Проверка конфига
        if (!ConverterJSON::fileConfigVerify()) {
            cerr << "Config verification failed. Aborting." << endl;
            return 1;
        }

        // Чтение документов и запросов
        auto docs = ConverterJSON::GetTextDocuments();
        auto requests = ConverterJSON::GetRequests();
        int maxResponses = ConverterJSON::GetResponsesLimit();

        // Отладочный вывод
        cout << "Docs loaded: " << docs.size() << ", Requests: " << requests.size() << ", maxResponses: " << maxResponses << endl;
        if (requests.empty()) {
            cout << "Warning: No requests loaded. Creating empty answers.json." << endl;
        }
        if (docs.empty()) {
            cout << "Warning: No documents loaded. Search will return false for all." << endl;
        }

        // Построение индекса
        InvertedIndex idx;
        idx.UpdateDocumentBase(docs);

        // Создание SearchServer (передаём лимит)
        SearchServer server(std::move(idx), maxResponses);

        // Запуск поиска
        auto raw = server.search(requests);

        // Конвертируем в формат для putAnswers: vector<vector<pair<int,float>>>
        vector<vector<pair<int, float>>> answers;
        answers.reserve(raw.size());
        for (const auto& block : raw) {
            vector<pair<int, float>> b;
            b.reserve(block.size());
            for (const auto& ri : block) {
                // приводим doc_id (size_t) к int безопасно
                b.emplace_back(static_cast<int>(ri.doc_id), ri.rank);
            }
            answers.push_back(std::move(b));
        }

        // Печать текущей рабочей директории
        cout << "Current working directory: " << fs::current_path().string() << endl;

        // Запись результатов (putAnswers сам пишет в answers.json в CWD)
        ConverterJSON::putAnswers(answers);

        cout << "Main finished normally." << endl;
        return 0;
    }
    catch (const std::exception& e) {
        cerr << "Unhandled exception in main: " << e.what() << endl;
        return 3; // возвращаем 3 чтобы было понятно, что было исключение
    }
    catch (...) {
        cerr << "Unhandled unknown exception in main." << endl;
        return 3;
    }
}
