#ifndef SYSTEM_HANDLER_HPP
#define SYSTEM_HANDLER_HPP

#include "WindowTracker.hpp"
#include <memory>

class SystemHandler {
public:
    explicit SystemHandler(const std::string& logDir);
    void tick();

private:
    std::unique_ptr<WindowTracker> tracker;
};

#endif //SYSTEM_HANDLER_HPP
