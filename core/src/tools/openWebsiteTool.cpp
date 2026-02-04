#include "tools/openWebsiteTool.h"
#include <windows.h>
#include <string>

agent::ToolResult OpenWebsiteTool::run(const json& args) {
#ifdef _WIN32

    if (!args.contains("url")) {
        agent::ToolResult result;
        result.output = "missing required argument: url";
        result.success = false;
        return result;
    }

    std::string url = args["url"];

    // Auto-add https:// if missing
    if (url.find("http://") != 0 && url.find("https://") != 0) {
        url = "https://" + url;
    }

    // Open in default browser
    HINSTANCE h = ShellExecuteA(
        NULL,
        "open",
        url.c_str(),
        NULL,
        NULL,
        SW_SHOWNORMAL
    );

    if ((INT_PTR)h <= 32) {
        agent::ToolResult result;
        result.output = "Failed to open website: " + url;
        result.success = false;
        return result;
    }

    agent::ToolResult result;
    result.output = "Opened website: " + url;
    result.success = true;
    return result;

#else
    return "OpenWebsiteTool only supported on Windows.";
#endif
}
