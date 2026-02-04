#include "tools/shutdownTool.h"
#include <cstdlib>

agent::ToolResult ShutdownTool::run(const json&) {
#ifdef _WIN32
    system("shutdown /s /t 5");
    agent::ToolResult result;
    result.output = "Shutting down in 5 seconds...";
    result.success = true;
    return result;
#else
    return "ShutdownTool only works on Windows.";
#endif
}
