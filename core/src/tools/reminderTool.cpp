#include "tools/reminderTool.h"
#include <fstream>
#include <iostream>
#include <ctime>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

static std::string current_time() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

std::string ReminderTool::run(const std::string& input) {

    const std::string filename = "..\\..\\core\\data\\tools\\savedTasks.json";

    json data;

    // Load existing JSON if file exists
    std::ifstream inFile(filename);

    if (inFile.is_open()) {
        try {
            inFile >> data;
        }
        catch (...) {
            // file exists but is corrupted
            data = json::object();
        }
        inFile.close();
    }

    // Ensure structure exists
    if (!data.contains("reminders")) {
        data["reminders"] = json::array();
    }

    // Add new reminder object
    json reminder;
    reminder["task"] = input;
    reminder["created_at"] = current_time();

    data["reminders"].push_back(reminder);

    // Save back clean JSON
    std::ofstream outFile(filename);
    outFile << data.dump(4); // pretty print with indent
    outFile.close();

    return "✅ Reminder saved: " + input;
}
