#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "tools/tool.h"
#include "agent/types.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class ToolRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<Tool>> tools;

public:
    void registerTool(std::unique_ptr<Tool> tool);

    bool hasTool(const std::string& name);
    ToolRisk toolRisk(const std::string& name) const;

    bool validateArgs(
        const std::string& toolName,
        const nlohmann::json& args
    );


    agent::ToolResult runTool(const std::string& name, const json& args);

    std::string listTools();
};
