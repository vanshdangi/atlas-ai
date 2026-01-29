#include "scheduler/taskScheduler.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

TaskScheduler::TaskScheduler(const std::string& file)
    : storageFile(file), running(false) {
    loadTasks();
}

void TaskScheduler::start() {
    running = true;
    worker = std::thread(&TaskScheduler::runLoop, this);
}

void TaskScheduler::stop() {
    running = false;
    if (worker.joinable())
        worker.join();
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

            std::cout << "\n⏰ Reminder: " << task.message << "\n";

            task.completed = true;
            saveTasks();
        }
    }
}

void TaskScheduler::addTask(const ScheduledTask& task) {
    tasks.push_back(task);
    saveTasks();
}
