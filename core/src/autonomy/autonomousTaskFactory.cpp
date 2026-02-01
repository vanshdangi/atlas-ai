#include "autonomy/autonomousTaskFactory.h"
#include <chrono>

static long long nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

ScheduledTask AutonomousTaskFactory::createTask(const std::string& triggerType) {

    ScheduledTask task;
    task.id = "auto_" + triggerType + "_" + std::to_string(nowMs());
    task.completed = false;

    // Run immediately
    task.dueTime = std::time(nullptr);

    // ---- Trigger → Task Mapping ----

    if (triggerType == "idle_user") {
        task.type = "tool";
        task.message = "User idle detected";

        task.toolCall = {
            {"tool", "speak"},
            {"args", {
                {"text", "You seem inactive Vansh. Want to start focus mode?"}
            }}
        };
    }

    else if (triggerType == "battery_low") {
        task.type = "tool";
        task.message = "Battery low autonomy event";

        task.toolCall = {
            {"tool", "enable_battery_saver"},
            {"args", json::object()}
        };
    }

    else {
        task.type = "reminder";
        task.message = "Unknown autonomous trigger fired";
        task.toolCall = nullptr;
    }

    return task;
}
