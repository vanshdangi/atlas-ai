#include "memory/memoryManager.h"

MemoryManager::MemoryManager(const std::string& data_dir) {
    fact_path = data_dir + "/memory/facts.json";
}

void MemoryManager::load() {
    facts.load(fact_path);
}

void MemoryManager::save() {
    facts.save(fact_path);
}
