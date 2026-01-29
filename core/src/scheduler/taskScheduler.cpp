#include "scheduler/taskScheduler.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

TaskScheduler::TaskScheduler(const std::string& file)
    : storageFile(file), running(false) {
    loadTasks();
}

void TaskScheduler::start() {
    if (running) return;

    running = true;
    worker = std::thread(&TaskScheduler::runLoop, this);
}

void TaskScheduler::stop() {
    running = false;
    if (worker.joinable())
        worker.join();

    saveTasks();
}

void TaskScheduler::runLoop() {
    while (running) {
        checkDueTasks();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void TaskScheduler::checkDueTasks() {
    long long now = std::time(nullptr);

    for (auto& task : tasks) {
        if (!task.completed && now >= task.dueTime) {

            std::cout << "\n Reminder: " << task.message << "\n";

            task.completed = true;
            saveTasks();
        }
    }
}

void TaskScheduler::addTask(const ScheduledTask& task) {
    tasks.push_back(task);
    saveTasks();
}

// =======================================================
// ✅ LOAD TASKS FROM JSON FILE
// =======================================================
void TaskScheduler::loadTasks() {
    std::ifstream inFile(storageFile);

    if (!inFile.is_open()) {
        std::cout << "[TaskScheduler] No task file found. Starting fresh.\n";
        return;
    }

    json data;
    try {
        inFile >> data;
    }
    catch (...) {
        std::cerr << "[TaskScheduler] Failed to parse tasks file.\n";
        return;
    }

    if (!data.contains("tasks")) return;

    tasks.clear();

    for (auto& t : data["tasks"]) {
        ScheduledTask task;
        task.id = t["id"];
        task.type = t["type"];
        task.message = t["message"];
        task.dueTime = t["dueTime"];
        task.completed = t["completed"];

        tasks.push_back(task);
    }

    std::cout << "[TaskScheduler] Loaded " << tasks.size() << " tasks.\n";
}

// =======================================================
// ✅ SAVE TASKS TO JSON FILE
// =======================================================
void TaskScheduler::saveTasks() {
    json data;
    data["tasks"] = json::array();

    for (auto& task : tasks) {
        json t;
        t["id"] = task.id;
        t["type"] = task.type;
        t["message"] = task.message;
        t["dueTime"] = task.dueTime;
        t["completed"] = task.completed;

        data["tasks"].push_back(t);
    }

    std::ofstream outFile(storageFile);
    outFile << data.dump(4);

    std::cout << "[TaskScheduler] Tasks saved.\n";
}