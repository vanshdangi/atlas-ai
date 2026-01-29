#pragma once
#include <string>

struct ScheduledTask {
    std::string id;
    std::string type;     // reminder/tool
    std::string message;

    std::string toolName;
    std::string toolArgs;

    long long dueTime;
    bool completed = false;
};
