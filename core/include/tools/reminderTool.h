#pragma once
#include "tools/tool.h"
#include "tools/scheduledTask.h"
#include "scheduler/taskScheduler.h"

class ReminderTool : public Tool {
public:
    ReminderTool(TaskScheduler& scheduler);
    
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

private:
    TaskScheduler& scheduler;
};
