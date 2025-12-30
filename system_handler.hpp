#ifndef SYSTEM_HANDLER_HPP
#define SYSTEM_HANDLER_HPP

#include "window_tracker.hpp"
#include <atomic>
#include <thread>
#include <memory>

class SystemHandler {
public:
    explicit SystemHandler(const std::string& logDir);
    ~SystemHandler();

    void start();
    void stop();
    bool isRunning() const;

private:
    void runLoop();

    std::unique_ptr<WindowTracker> tracker;
    std::thread worker;
    std::atomic<bool> running{false};
};

#endif //SYSTEM_HANDLER_HPP
