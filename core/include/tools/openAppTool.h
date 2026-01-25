#pragma once
#include "tools/tool.h"

class OpenAppTool : public Tool {
public:
    std::string name() const override {
        return "open_app";
    }

    std::string description() const override {
        return "Opens an application like chrome, spotify, vscode.";
    }

    std::string run(const std::string& input) override;
};
