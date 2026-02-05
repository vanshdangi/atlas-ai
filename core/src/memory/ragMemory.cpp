#include "memory/ragMemory.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

RagMemory::RagMemory(const std::string& file)
    : storeFile(file) {
    load();
}

void RagMemory::load() {
    std::ifstream in(storeFile);
    if (!in.is_open()) return;

    json j;
    in >> j;

    for (auto& item : j) {
        MemoryChunk m;
        m.id = item["id"];
        m.timestamp = item["timestamp"];
        m.text = item["text"];
        m.tag = item["tag"];
        memories.push_back(m);
    }
}

void RagMemory::persist() {
    json j = json::array();

    for (auto& m : memories) {
        j.push_back({
            {"id", m.id},
            {"timestamp", m.timestamp},
            {"text", m.text},
            {"tag", m.tag}
        });
    }

    std::ofstream out(storeFile);
    out << j.dump(4);
}

void RagMemory::addMemory(const MemoryChunk& chunk) {
    memories.push_back(chunk);
    persist();
}

std::vector<MemoryChunk> RagMemory::search(const std::string& query) {
    std::vector<MemoryChunk> results;

    for (auto& m : memories) {
        if (m.text.find(query) != std::string::npos)
            results.push_back(m);
    }

    return results;
}
