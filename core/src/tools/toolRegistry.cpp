#include "tools/toolRegistry.h"
#include <sstream>

void ToolRegistry::registerTool(std::unique_ptr<Tool> tool) {
    tools[tool->name()] = std::move(tool);
}

bool ToolRegistry::hasTool(const std::string& name) {
    return tools.find(name) != tools.end();
}

std::string ToolRegistry::runTool(const std::string& name, const std::string& input) {
    if (!hasTool(name)) {
        return "Tool not found.";
    }
    return tools[name]->run(input);
}

std::string ToolRegistry::listTools() {
    std::stringstream ss;

    ss << "Available tools:\n";
    for (auto& pair : tools) {
        ss << "- " << pair.first << ": " << pair.second->description() << "\n";
    }

    return ss.str();
}
