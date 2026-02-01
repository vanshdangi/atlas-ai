#include "autonomy/triggerManager.h"

void TriggerManager::addTrigger(std::unique_ptr<Trigger> trigger) {
    triggers.push_back(std::move(trigger));
}

void TriggerManager::pollAll() {
    for (auto& t : triggers)
        t->poll();
}

std::vector<Trigger*> TriggerManager::firedTriggers() {
    std::vector<Trigger*> out;

    for (auto& t : triggers) {
        if (t->fired())
            out.push_back(t.get());
    }

    return out;
}
