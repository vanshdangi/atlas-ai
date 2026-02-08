#pragma once
#include "autonomy/triggerManager.h"
#include "scheduler/taskScheduler.h"

class AutonomousSystem {
public:
    AutonomousSystem(TaskScheduler& scheduler);

    void start();
    void stop();

private:
    bool running = true;
    TaskScheduler& scheduler;
    std::thread worker;

    TriggerManager triggers;

    void loop();
};
