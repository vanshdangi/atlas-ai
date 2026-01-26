#pragma once
#include "tools/tool.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class OpenAppTool : public Tool {
public:
    std::string name() const override {
        return "open_app";
    }

    std::string description() const override {
        return "Opens an application like chrome, spotify, vscode.";
    }

    std::string run(const json& args) override;
};
