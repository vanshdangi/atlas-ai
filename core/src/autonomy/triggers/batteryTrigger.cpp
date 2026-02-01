#include "autonomy/triggers/batteryTrigger.h"
#include <windows.h>

void BatteryTrigger::poll() {

    SYSTEM_POWER_STATUS status;
    GetSystemPowerStatus(&status);

    if (status.BatteryLifePercent <= 20) {
        triggered = true;
    }
}

bool BatteryTrigger::fired() const {
    return triggered;
}

std::string BatteryTrigger::triggerType() const {
    return "battery_low";
}

void BatteryTrigger::reset() {
    triggered = false;
}
