#include "tools/reminderTool.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>

static long long now_unix() {
    return static_cast<long long>(std::time(nullptr));
}
static long long parse_datetime_to_unix(const std::string& datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime);

    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");

    if (ss.fail()) {
        return -1; // invalid format
    }

    return std::mktime(&tm); // converts local time → unix
}
static std::string unix_to_datetime(long long unix_time) {
    std::time_t t = static_cast<std::time_t>(unix_time);
    std::tm* tm_ptr = std::localtime(&t);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", tm_ptr);

    return std::string(buffer);
}
static std::string seconds_to_human(long long seconds) {

    if (seconds < 60)
        return std::to_string(seconds) + " seconds";

    long long minutes = seconds / 60;
    long long remSec = seconds % 60;

    if (minutes < 60)
        return std::to_string(minutes) + " minutes";

    long long hours = minutes / 60;
    long long remMin = minutes % 60;

    if (hours < 24)
        return std::to_string(hours) + " hours " +
                std::to_string(remMin) + " minutes";

    long long days = hours / 24;
    long long remHr = hours % 24;

    return std::to_string(days) + " days " +
            std::to_string(remHr) + " hours";
}

ReminderTool::ReminderTool(TaskScheduler& scheduler)
    : scheduler(scheduler) {}

std::string ReminderTool::run(const json& args) {
    if (!args.contains("text")) {
        return "Reminder failed: Missing reminder text.";
    }
    std::string message = args["text"];

    long long due_time = now_unix() + 60; // default

    if (args.contains("time")) {
        std::string timeStr = args["time"];
        long long parsed = parse_datetime_to_unix(timeStr);
        if (parsed == -1) {
            return "Reminder failed: Invalid time format.";
        }
        due_time = parsed;
    }
    else if (args.contains("delay_minutes")) {
        due_time =  now_unix() + args["delay_minutes"].get<int>() * 60;
    }
    else if (args.contains("delay_seconds")) {
        due_time = now_unix() + args["delay_seconds"].get<int>();
    }

    
    ScheduledTask task;
    task.id = "task_" + std::to_string(std::rand());
    task.type = "reminder";
    task.message = message;
    task.dueTime = due_time;
    task.completed = false;
    
    scheduler.addTask(task);

    long long now = now_unix();
    long long diff = task.dueTime - now;

    return "Reminder set for " +
        unix_to_datetime(task.dueTime) +
        " (" + seconds_to_human(diff) + " from now): " +
        message;
}