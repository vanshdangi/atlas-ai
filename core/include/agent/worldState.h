#pragma once
#include <string>

struct WorldSnapshot {
    std::string time;
    std::string date;
    std::string day;
    int battery_percent;
    bool charging;
    bool idle;
};

class WorldState {
public:
    WorldSnapshot capture();
    std::string to_prompt_block();
};