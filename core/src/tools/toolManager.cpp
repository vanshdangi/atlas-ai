#include "tools/toolManager.h"
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void ToolManager::registerAllTools(ToolRegistry& registry){
    registry.registerTool(std::make_unique<OpenAppTool>());
    registry.registerTool(std::make_unique<ShutdownTool>());
    registry.registerTool(std::make_unique<ReminderTool>());
}

bool ToolManager::isToolCall(const std::string& text) {
    return text.find("\"tool\"") != std::string::npos;
}

std::string ToolManager::executeToolCall(
    const std::string& jsonText,
    ToolRegistry& registry
) {
    try {
        json call = json::parse(jsonText);

        // Tool name
        if (!call.contains("tool")) {
            return "Tool call missing 'tool'.";
        }

        std::string tool = call["tool"];

        json args = json::object();

        if (call.contains("args")) {
            if (call["args"].is_string()) {
                args["input"] = call["args"];
            }
            else {
                args = call["args"];
            }
        }
        
        if (tool == "list_tools") {
            return R"(Available tools:
- open_app
- create_reminder
- shutdown_pc)";
        }

        // Dispatch via registry
        return registry.runTool(tool, args);
    }
    catch (const std::exception& e) {
        return std::string("Tool JSON parse error: ") + e.what();
    }
}
