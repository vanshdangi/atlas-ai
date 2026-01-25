#pragma once
#include <string>
#include "tools/toolRegistry.h"
#include "tools/openAppTool.h"
#include "tools/shutdownTool.h"
#include "tools/reminderTool.h"

class ToolManager {
public:
    static void registerAllTools(ToolRegistry& registry);
    static bool isToolCall(const std::string& text);
    static std::string executeToolCall(const std::string& jsonText, ToolRegistry& registry);
};
