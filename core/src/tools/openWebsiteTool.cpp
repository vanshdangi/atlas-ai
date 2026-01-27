#include "tools/openWebsiteTool.h"
#include <windows.h>
#include <string>

std::string OpenWebsiteTool::run(const json& args) {
#ifdef _WIN32

    if (!args.contains("url")) {
        return "Missing required argument: url";
    }

    std::string url = args["url"];

    // Auto-add https:// if missing
    if (url.find("http://") != 0 && url.find("https://") != 0) {
        url = "https://" + url;
    }

    // Open in default browser
    HINSTANCE result = ShellExecuteA(
        NULL,
        "open",
        url.c_str(),
        NULL,
        NULL,
        SW_SHOWNORMAL
    );

    if ((INT_PTR)result <= 32) {
        return "Failed to open website: " + url;
    }

    return "Opened website: " + url;

#else
    return "OpenWebsiteTool only supported on Windows.";
#endif
}
