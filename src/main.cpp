#include "converterjson.h"
#include "invertedindex.h"
#include "searchserver.h"
#include <iostream>

int main(int argc, char *argv[]) {
    ConverterJSON converter;

    if (argc > 1) {
        converter.setConfigPath(argv[1]);
    }

    std::vector<std::string> documents = converter.getTextDocuments();
    InvertedIndex index;
    index.UpdateDocumentBase(documents);

    SearchServer srv(index);
    std::vector<std::string> requests = converter.getRequests();
    int maxResponses = converter.getResponsesLimit();

    auto answers = srv.search(requests);
    converter.putAnswers(answers);

    std::cout << "Search completed. Results saved to answers.json" << std::endl;
    return 0;
}
