#pragma once
#include "autonomy/trigger.h"
#include <vector>
#include <memory>

class TriggerManager {
public:
    void addTrigger(std::unique_ptr<Trigger> trigger);

    void pollAll();
    std::vector<Trigger*> firedTriggers();

private:
    std::vector<std::unique_ptr<Trigger>> triggers;
};
