#pragma once
#include <string>
#include "tools/toolRegistry.h"
#include "tools/openAppTool.h"
#include "tools/shutdownTool.h"
#include "tools/reminderTool.h"
#include "tools/openWebsiteTool.h"

class TaskScheduler;  // Forward declaration

class ToolManager {
public:
    static void registerAllTools(
        ToolRegistry& registry,
        TaskScheduler& scheduler
    );
    static bool isToolCall(const std::string& text);
    static agent::ToolResult executeToolCall(const std::string& jsonText, ToolRegistry& registry);
};
