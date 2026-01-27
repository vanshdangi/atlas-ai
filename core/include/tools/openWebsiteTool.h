#pragma once
#include "tools/tool.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class OpenWebsiteTool : public Tool {
public:
    std::string name() const override {
        return "open_website";
    }

    std::string description() const override {
        return "Opens a website";
    }

    std::string run(const json& args) override;
};