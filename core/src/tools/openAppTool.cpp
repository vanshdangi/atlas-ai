#include "tools/openAppTool.h"
#include <windows.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

std::string normalize(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    // Remove punctuation
    s.erase(std::remove_if(s.begin(), s.end(),
        [](char c) { return std::ispunct((unsigned char)c); }),
        s.end()
    );

    // Replace multiple spaces with one
    s.erase(std::unique(s.begin(), s.end(),
        [](char a, char b) { return a == ' ' && b == ' '; }),
        s.end()
    );

    return s;
}

std::string findAppPath(const std::string& inputName) {
    std::ifstream file("..\\..\\core\\data\\config\\apps.json");
    if (!file.is_open()) return "";

    json apps;
    file >> apps;

    std::string query = normalize(inputName);

    for (auto& [key, data] : apps.items()) {

        if (!data.contains("aliases") || !data.contains("path"))
            continue;

        for (auto& alias : data["aliases"]) {

            std::string aliasStr = alias.get<std::string>();

            if (normalize(aliasStr) == query) {
                return data["path"].get<std::string>();
            }
        }
    }

    return "";
}

agent::ToolResult OpenAppTool::run(const json& args) {
#ifdef _WIN32

    if (!args.contains("app")) {
        agent::ToolResult result;
        result.output = "Missing required argument: app";
        result.success = false;
        return result;
    }

    std::string appName = args["app"];

    std::string path = findAppPath(appName);

    if (path.empty()) {
        agent::ToolResult result;
        result.output = "I don't know where " + appName + " is installed.";
        result.success = false;
        return result;
    }

    HINSTANCE h = ShellExecuteA(
        NULL, "open",
        path.c_str(),
        NULL, NULL,
        SW_SHOWNORMAL
    );

    if ((INT_PTR)h <= 32) {
        agent::ToolResult result;
        result.output = "Failed to launch: " + appName;
        result.success = false;
        return result;
    }

    agent::ToolResult result;
    result.output = "Opened: " + appName;
    result.success = true;
    return result;

#else
    return "OpenAppTool only implemented on Windows.";
#endif
}