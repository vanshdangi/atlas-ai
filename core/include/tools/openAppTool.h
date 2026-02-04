#pragma once
#include "tools/tool.h"
#include "nlohmann/json.hpp"
#include "agent/types.h"
using json = nlohmann::json;

class OpenAppTool : public Tool {
public:
    std::string name() const override {
        return "open_app";
    }

    std::string description() const override {
        return "open_app(app: string)";
    }

    ToolRisk risk() const override {
        return ToolRisk::SAFE;
    }

    std::vector<std::string> requiredArgs() const override {
        return {"app"};
    }

    agent::ToolResult run(const json& args) override;
};
