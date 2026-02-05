#include "memory/summaryMemory.h"
#include <fstream>

SummaryMemory::SummaryMemory(const std::string& path)
    : filePath(path) {}

std::string SummaryMemory::loadSummary() {
    std::ifstream in(filePath);
    if (!in.is_open()) return "";

    std::string summary((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
    return summary;
}

void SummaryMemory::saveSummary(const std::string& text) {
    std::ofstream out(filePath);
    out << text;
}
