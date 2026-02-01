#include "autonomy/triggers/idleTrigger.h"
#include <windows.h>

IdleTrigger::IdleTrigger(int idleSeconds)
    : thresholdSeconds(idleSeconds)
{}

void IdleTrigger::poll() {
    LASTINPUTINFO li = { sizeof(LASTINPUTINFO) };
    if (!GetLastInputInfo(&li))
        return;

    DWORD nowTicks = GetTickCount();
    DWORD idleMs = nowTicks - li.dwTime;
    int idleSeconds = static_cast<int>(idleMs / 1000);

    if (idleSeconds < thresholdSeconds) {
        armed = true;   // user was active; allow next fire when they go idle
    } else if (armed) {
        triggered = true;
        armed = false;  // don't fire again until they're active and idle again
    }
}

bool IdleTrigger::fired() const {
    return triggered;
}

std::string IdleTrigger::triggerType() const {
    return "idle_user";
}

void IdleTrigger::reset() {
    triggered = false;
}
