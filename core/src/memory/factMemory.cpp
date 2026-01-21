#include "memory/factMemory.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

void FactMemory::load(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return;

    json j;
    f >> j;
    facts = j.get<std::unordered_map<std::string, std::string>>();
}

void FactMemory::save(const std::string& path) const {
    json j = facts;
    std::ofstream f(path);
    f << j.dump(4);
}

void FactMemory::set(const std::string& key, const std::string& value) {
    facts[key] = value;
}

bool FactMemory::has(const std::string& key) const {
    return facts.count(key);
}

std::string FactMemory::get(const std::string& key) const {
    return facts.at(key);
}

std::string FactMemory::to_prompt_block() const {
    if (facts.empty()) return "";

    std::string block = "Known facts about the user:\n";
    for (auto& [k, v] : facts) {
        block += "- " + k + ": " + v + "\n";
    }
    return block;
}
