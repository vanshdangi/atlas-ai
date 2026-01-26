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

std::string OpenAppTool::run(const json& args) {
#ifdef _WIN32

    if (!args.contains("app")) {
        return "Missing required argument: app";
    }

    std::string appName = args["app"];

    std::string path = findAppPath(appName);

    if (path.empty()) {
        return "I don't know where " + appName + " is installed.";
    }

    HINSTANCE result = ShellExecuteA(
        NULL, "open",
        path.c_str(),
        NULL, NULL,
        SW_SHOWNORMAL
    );

    if ((INT_PTR)result <= 32) {
        return "Failed to launch: " + appName;
    }

    return "Opened: " + appName;

#else
    return "OpenAppTool only implemented on Windows.";
#endif
}