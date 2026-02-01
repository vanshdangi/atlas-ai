#pragma once
#include "autonomy/trigger.h"

class BatteryTrigger : public Trigger {
public:
    void poll() override;
    bool fired() const override;

    std::string triggerType() const override;
    void reset() override;

private:
    bool triggered = false;
};
