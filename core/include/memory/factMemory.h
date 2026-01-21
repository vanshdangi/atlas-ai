#pragma once
#include <unordered_map>
#include <string>

class FactMemory {
public:
    void load(const std::string& path);
    void save(const std::string& path) const;

    void set(const std::string& key, const std::string& value);
    bool has(const std::string& key) const;
    std::string get(const std::string& key) const;

    std::string to_prompt_block() const;

private:
    std::unordered_map<std::string, std::string> facts;
};
