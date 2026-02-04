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
        return "create_reminder(text: string, delay_minutes?: int, time?: string)";
    }

    ToolRisk risk() const override {
        return ToolRisk::SAFE;
    }

    std::vector<std::string> requiredArgs() const override {
        return {"text"};
    }

    std::string run(const json& args) override;

private:
    TaskScheduler& scheduler;
};
