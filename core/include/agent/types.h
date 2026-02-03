#pragma once

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace agent {

struct Step {
    int id = 0;
    std::string description;
    std::string tool;
    json args = json::object();
};

struct Plan {
    std::vector<Step> steps;
};

struct ToolResult {
    bool success = false;
    std::string output;
};

struct ExecutionResult {
    int step_id = 0;
    ToolResult tool_result;
    bool success = false;
};

struct ReflectionResult {
    bool needs_retry = false;
    bool needs_clarification = false;
    std::string question;
    std::string fallback_tool;
};

} // namespace agent
