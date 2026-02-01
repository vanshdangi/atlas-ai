#pragma once
#include "tools/scheduledTask.h"

class AutonomousTaskFactory {
public:
    static ScheduledTask createTask(const std::string& triggerType);
};