#include "tools/toolManager.h"
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void ToolManager::registerAllTools(
    ToolRegistry& registry,
    TaskScheduler& scheduler
){
    registry.registerTool(std::make_unique<OpenAppTool>());
    registry.registerTool(std::make_unique<ShutdownTool>());
    registry.registerTool(std::make_unique<ReminderTool>(scheduler));
    registry.registerTool(std::make_unique<OpenWebsiteTool>());
}

bool ToolManager::isToolCall(const std::string& text) {
    return text.find("\"tool\"") != std::string::npos;
}

agent::ToolResult ToolManager::executeToolCall(
    const std::string& jsonText,
    ToolRegistry& registry
) {
    try {
        json call = json::parse(jsonText);

        // Tool name
        if (!call.contains("tool")) {
            agent::ToolResult result;
            result.output = "Tool call missing 'tool'.";
            result.success = false;
            return result;
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
        // Dispatch via registry
        return registry.runTool(tool, args);
    }
    catch (const std::exception& e) {
        agent::ToolResult result;
        result.output = std::string("Tool JSON parse error: ") + e.what();
        result.success = false;
        return result;
    }
}
