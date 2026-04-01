#include "SystemHandler.hpp"
#include <chrono>

SystemHandler::SystemHandler(const std::string& logDir) {
    tracker = std::make_unique<WindowTracker>(logDir);
}

void SystemHandler::tick() {
    tracker->tick();
}
