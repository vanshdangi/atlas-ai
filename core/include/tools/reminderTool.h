#pragma once
#include "tools/tool.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class ReminderTool : public Tool {
public:
    std::string name() const override {
        return "create_reminder";
    }

    std::string description() const override {
        return "Creates a reminder and saves it into tasks.json.";
    }

    ToolRisk risk() const override {
        return ToolRisk::SAFE;
    }

    std::string run(const json& args) override;
};
