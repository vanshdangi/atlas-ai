#pragma once
#include "autonomy/trigger.h"

class IdleTrigger : public Trigger {
public:
    IdleTrigger(int idleSeconds);

    void poll() override;
    bool fired() const override;

    std::string triggerType() const override;
    void reset() override;

private:
    int thresholdSeconds;
    bool triggered = false;
    bool armed = true;   // re-arm only after user is active again (idle < threshold)
};
