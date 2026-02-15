#include "agent/worldState.h"
#include <windows.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

// -------------------- Helpers --------------------

static std::string get_time_string() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local{};
    localtime_s(&local, &now_c);

    std::ostringstream oss;
    oss << std::put_time(&local, "%H:%M:%S");
    return oss.str();
}

static std::string get_date_string() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local{};
    localtime_s(&local, &now_c);

    std::ostringstream oss;
    oss << std::put_time(&local, "%Y-%m-%d");
    return oss.str();
}

static std::string get_day_string() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local{};
    localtime_s(&local, &now_c);

    std::ostringstream oss;
    oss << std::put_time(&local, "%A");
    return oss.str();
}

static int get_battery_percent() {
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status)) {
        return status.BatteryLifePercent;
    }
    return -1;
}

static bool is_charging() {
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status)) {
        return status.ACLineStatus == 1;
    }
    return false;
}

static bool is_idle() {
    LASTINPUTINFO lii;
    lii.cbSize = sizeof(LASTINPUTINFO);
    GetLastInputInfo(&lii);

    DWORD tickCount = GetTickCount();
    DWORD idleTime = tickCount - lii.dwTime;

    // idle if more than 2 minutes
    return idleTime > 120000;
}

// -------------------- Implementation --------------------

WorldSnapshot WorldState::capture() {
    WorldSnapshot snap;
    snap.time = get_time_string();
    snap.date = get_date_string();
    snap.day = get_day_string();
    snap.battery_percent = get_battery_percent();
    snap.charging = is_charging();
    snap.idle = is_idle();
    return snap;
}

std::string WorldState::to_prompt_block() {
    WorldSnapshot snap = capture();

    std::string block;
    block += "<|start_header_id|>world_state<|end_header_id|>\n";
    block += "Date: " + snap.date + "\n";
    block += "Day: " + snap.day + "\n";
    block += "Time: " + snap.time + "\n";

    if (snap.battery_percent >= 0) {
        block += "Battery: " + std::to_string(snap.battery_percent) + "%\n";
        block += "Charging: ";
        block += (snap.charging ? "Yes\n" : "No\n");
    }

    block += "User Idle: ";
    block += (snap.idle ? "Yes\n" : "No\n");

    block += "<|eot_id|>\n";
    return block;
}
