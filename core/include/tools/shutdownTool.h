#pragma once
#include "tools/tool.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class ShutdownTool : public Tool {
public:
    std::string name() const override {
        return "shutdown_pc";
    }

    std::string description() const override {
        return "shutdown_pc()";
    }

    ToolRisk risk() const override {
        return ToolRisk::CRITICAL;
    }
    
    std::vector<std::string> requiredArgs() const override {
        return {};
    }

    std::string run(const json& args) override;
};
