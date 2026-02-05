#pragma once
#include <string>

class SummaryMemory {
public:
    SummaryMemory(const std::string& path);

    std::string loadSummary();
    void saveSummary(const std::string& text);

private:
    std::string filePath;
};
