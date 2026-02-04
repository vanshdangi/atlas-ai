#include "tools/toolRegistry.h"
#include <sstream>
#include "nlohmann/json.hpp"
#include <iostream>
using json = nlohmann::json;

void ToolRegistry::registerTool(std::unique_ptr<Tool> tool) {
    tools[tool->name()] = std::move(tool);
}

bool ToolRegistry::hasTool(const std::string& name) {
    return tools.find(name) != tools.end();
}

bool ToolRegistry::validateArgs(
    const std::string& toolName,
    const json& args
) {
    if (!hasTool(toolName)) {
        return false;
    }

    Tool* tool = tools[toolName].get();

    // Get required keys
    auto required = tool->requiredArgs();

    // If tool needs no args → valid
    if (required.empty()) return true;

    // Args must be JSON object
    if (!args.is_object()) {
        std::cout << "Args must be an object for tool: "
                << toolName << "\n";
        return false;
    }

    // Check required keys exist
    for (const auto& key : required) {

        if (!args.contains(key)) {
            std::cout << "Missing required arg: "
                    << key
                    << " for tool: "
                    << toolName
                    << "\n";
            return false;
        }

        // Reject null values
        if (args[key].is_null()) {
            std::cout << "Arg is null: "
                    << key
                    << " for tool: "
                    << toolName
                    << "\n";
            return false;
        }
    }

    return true;
}


agent::ToolResult ToolRegistry::runTool(const std::string& name, const json& args) {
    if (!hasTool(name)) {
        agent::ToolResult result;
        result.output = "Tool not found.";
        result.success = false;
        return result;
    }

    Tool* tool = tools[name].get();
    ToolRisk risk = tool->risk();

    if (risk == ToolRisk::SAFE) {
        return tool->run(args);
    }

    if (risk == ToolRisk::CONFIRM) {
        std::cout << "\n Confirmation required for tool: " << name << "\n";
        std::cout << "Type yes to confirm: ";

        std::string input;
        std::getline(std::cin, input);

        if (input != "yes") {
            agent::ToolResult result;
            result.output = "Cancelled.";
            result.success = false;
            return result;
        }

        return tool->run(args);
    }

    if (risk == ToolRisk::CRITICAL) {
        std::cout << "\nCRITICAL TOOL REQUESTED: " << name << "\n";
        std::cout << "This may shut down your PC or delete files.\n";
        std::cout << "Type YES to confirm: ";

        std::string input;
        std::getline(std::cin, input);

        if (input != "yes") {
            agent::ToolResult result;
            result.output = "Blocked critical action.";
            result.success = false;
            return result;
        }

        return tool->run(args);
    }

    agent::ToolResult result;
    result.output = "Tool blocked: unknown risk level.";
    result.success = false;
    return result;
}

std::string ToolRegistry::listTools() {
    std::stringstream ss;
    for (auto& pair : tools) {
        ss << "- " << pair.second->description() << "\n";
    }

    return ss.str();
}
