#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include <vector>
#include <string>

struct RelativeIndex;

class ConverterJSON {
public:
    ConverterJSON();
    void setConfigPath(const std::string& path);
    std::vector<std::string> getTextDocuments() const;
    std::vector<std::string> getRequests() const;
    int getResponsesLimit() const;
    void putAnswers(const std::vector<std::vector<RelativeIndex>>& answers) const;

private:
    std::string configPath;
};

#endif
