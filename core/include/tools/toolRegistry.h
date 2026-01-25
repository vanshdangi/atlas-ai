#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "tools/tool.h"

class ToolRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<Tool>> tools;

public:
    void registerTool(std::unique_ptr<Tool> tool);

    bool hasTool(const std::string& name);

    std::string runTool(const std::string& name, const std::string& input);

    std::string listTools();
};
