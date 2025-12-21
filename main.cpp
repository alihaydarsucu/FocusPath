#include "app_usage_analyzer.hpp"
#include "window_tracker.hpp"
#include "log_parser.hpp"
#include <thread>
#include <chrono>

int main() {
    // WINDOW TRACKER

    WindowTracker tracker("window_log.json");

    while (true) {
        tracker.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // JSON PARSER

    WindowUsageParser parser("window_log.json");

    /*std::cout << "\nWriting total: \n";

    for (const auto& [app, ms] : parser.totals_by_app()) {
        std::cout << app << " → " << ms << " ms\n";
    }

    std::cout << "\nWriting events: \n";

    for (const auto& e : parser.events()) {
    std::cout << e.window_class << " " << e.duration_ms << " ms\n";
    }*/

    // LISTED VS UNLISTED TIME SPENT

    /*std::vector<std::string> trackedApps;
    trackedApps.emplace_back("Alacritty");

    AppUsageAnalyzer analyzer(parser.totals_by_app(), trackedApps);
    AppUsageAnalyzer::Result result = analyzer.analyze();

    // Output
    std::cout << "Listed apps:\n";
    std::cout << "  Time: " << result.listedTime / 1000 << " s\n";
    std::cout << "  Usage: " << result.listedPercent << " %\n";

    std::cout << "Unlisted apps:\n";
    std::cout << "  Time: " << result.unlistedTime / 1000 << " s\n";
    std::cout << "  Usage: " << result.unlistedPercent << " %\n";
    */
    return 0;
}
