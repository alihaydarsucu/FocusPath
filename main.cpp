#include "window_tracker2.hpp"
#include <thread>
#include <chrono>

int main() {
    WindowTracker tracker("window_log.json");

    while (true) {
        tracker.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}
