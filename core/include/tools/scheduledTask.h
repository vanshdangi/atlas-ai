#pragma once
#include <string>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct ScheduledTask {
    std::string id;
    std::string type;     // reminder/tool
    std::string message;

    json toolCall;

    long long dueTime;
    bool completed = false;
};
