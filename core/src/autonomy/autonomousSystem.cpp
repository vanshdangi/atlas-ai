#include "autonomy/autonomousSystem.h"
#include "autonomy/autonomousTaskFactory.h"

#include "autonomy/triggers/idleTrigger.h"
#include "autonomy/triggers/batteryTrigger.h"

#include <thread>
#include <iostream>

AutonomousSystem::AutonomousSystem(TaskScheduler& sched)
    : scheduler(sched)
{
    // Add triggers here
    triggers.addTrigger(std::make_unique<IdleTrigger>(60));
    triggers.addTrigger(std::make_unique<BatteryTrigger>());
}

void AutonomousSystem::start() {
    running = true;
    worker = std::thread([this]() { loop(); });
}

void AutonomousSystem::stop() {
    running = false;
    if (worker.joinable())
        worker.join();
}

void AutonomousSystem::loop() {

    while (running) {

        triggers.pollAll();

        auto fired = triggers.firedTriggers();

        for (auto* trigger : fired) {

            std::string type = trigger->triggerType();

            std::cout << "[AUTONOMY] Fired: " << type << "\n";

            // Create ScheduledTask
            ScheduledTask task =
                AutonomousTaskFactory::createTask(type);

            // Inject into scheduler
            scheduler.addTask(task);

            trigger->reset();
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(2)
        );
    }
}
