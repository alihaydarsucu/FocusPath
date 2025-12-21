#include "window_tracker2.hpp"
#include "log_parser.hpp"
#include <thread>
#include <chrono>

int main() {
    /*WindowTracker tracker("window_log.json");

    while (true) {
        tracker.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }*/

    WindowUsageParser parser("window_log.json");

    std::cout << "\nWriting total: \n";

    for (const auto& [app, ms] : parser.totals_by_app()) {
        std::cout << app << " → " << ms << " ms\n";
    }

    std::cout << "\nWriting events: \n";

    for (const auto& e : parser.events()) {
    std::cout << e.window_class << " " << e.duration_ms << " ms\n";
    }

    return 0;
}
