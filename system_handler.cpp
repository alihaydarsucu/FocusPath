#include "system_handler.hpp"
#include <chrono>

SystemHandler::SystemHandler(const std::string& logDir) {
    tracker = std::make_unique<WindowTracker>(logDir);
}

SystemHandler::~SystemHandler() {
    stop();
}

void SystemHandler::start() {
    if (running) return;

    running = true;
    worker = std::thread(&SystemHandler::runLoop, this);
}

void SystemHandler::stop() {
    if (!running) return;

    running = false;
    if (worker.joinable())
        worker.join();
}

bool SystemHandler::isRunning() const {
    return running;
}

void SystemHandler::runLoop() {
    using namespace std::chrono_literals;

    while (running) {
        tracker->tick();
        std::this_thread::sleep_for(200ms); // don’t melt the CPU
    }
}
