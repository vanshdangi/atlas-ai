#pragma once
#include <vector>
#include <string>
#include "memoryTypes.h"

class RagMemory {
public:
    RagMemory(const std::string& file);

    void addMemory(const MemoryChunk& chunk);
    std::vector<MemoryChunk> search(const std::string& query);

private:
    std::string storeFile;
    std::vector<MemoryChunk> memories;

    void load();
    void persist();
};
