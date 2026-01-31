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

std::string ToolRegistry::runTool(const std::string& name, const json& args) {
    if (!hasTool(name)) {
        return "Tool not found.";
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
            return "Cancelled.";
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
            return "Blocked critical action.";
        }

        return tool->run(args);
    }

    return "Tool blocked: unknown risk level.";
}

std::string ToolRegistry::listTools() {
    std::stringstream ss;

    ss << "Available tools:\n";
    for (auto& pair : tools) {
        ss << "- " << pair.first << ": " << pair.second->description() << "\n";
    }

    return ss.str();
}
