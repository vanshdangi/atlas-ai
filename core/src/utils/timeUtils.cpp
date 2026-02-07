#include "utils/timeUtils.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string get_current_time_iso() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm local{};
    localtime_s(&local, &t);

    std::ostringstream oss;
    oss << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string get_human_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm local{};
    localtime_s(&local, &t);

    std::ostringstream oss;
    oss << std::put_time(&local, "%I:%M %p");
    return oss.str();
}

std::string get_day_context() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm local{};
    localtime_s(&local, &t);

    std::ostringstream oss;
    oss << std::put_time(&local, "%A, %d %B %Y");
    return oss.str();
}

std::string build_time_context() {
    return
        "Current date: " + get_day_context() + "\n"
        "Current time: " + get_human_time() + "\n"
        "Timezone: Asia/Kolkata\n";
}