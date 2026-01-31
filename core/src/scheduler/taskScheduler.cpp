#include "scheduler/taskScheduler.h"
#include "tools/toolManager.h"
#include "tools/toolRegistry.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

TaskScheduler::TaskScheduler(const std::string& file, ToolManager& toolManager, ToolRegistry& toolRegistry, PiperTTS& piper)
    : storageFile(file), toolManager(toolManager), toolRegistry(toolRegistry), piper(piper), running(false) {
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
    while (running) {

        long long now = std::time(nullptr);

        for (auto& task : tasks) {
            if (!task.completed && task.dueTime <= now) {
                executeTask(task);
                task.completed = true;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void TaskScheduler::executeTask(ScheduledTask& task) {

    if (task.type == "reminder") {
        std::cout << "\nREMINDER: " << task.message << "\n";
        piper.speakAsync(task.message);
    }

    else if (task.type == "tool") {
        std::cout << "\nExecuting scheduled tool...\n";
        piper.speakAsync("Executing scheduled tool.");

        std::string jsonText = task.toolCall.dump();
        toolManager.executeToolCall(jsonText, toolRegistry);
    }
    std::cout << "\nYou: ";
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
        t["tool_call"] = task.toolCall;
        t["dueTime"] = task.dueTime;
        t["completed"] = task.completed;

        data["tasks"].push_back(t);
    }

    std::ofstream outFile(storageFile);
    outFile << data.dump(4);

    std::cout << "[TaskScheduler] Tasks saved.\n";
}