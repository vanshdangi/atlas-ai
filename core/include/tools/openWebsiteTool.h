#pragma once
#include "tools/tool.h"
#include <nlohmann/json.hpp>
#include "agent/types.h"

using json = nlohmann::json;

class OpenWebsiteTool : public Tool {
public:
    std::string name() const override {
        return "open_website";
    }

    std::string description() const override {
        return "open_website(url: string)";
    }

    ToolRisk risk() const override {
        return ToolRisk::SAFE;
    }

    std::vector<std::string> requiredArgs() const override {
        return {"url"};
    }

    agent::ToolResult run(const json& args) override;
};