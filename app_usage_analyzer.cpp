#include "app_usage_analyzer.hpp"

AppUsageAnalyzer::AppUsageAnalyzer(
    const UsageMap& usage,
    const std::vector<std::string>& trackedApps
) : usage_(usage),
    trackedApps_(trackedApps.begin(), trackedApps.end()) {}

AppUsageAnalyzer::AppUsageResult AppUsageAnalyzer::analyze() const {
    long long listed = 0;
    long long unlisted = 0;

    for (const auto& [app, time] : usage_) {
        (trackedApps_.contains(app) ? listed : unlisted) += time;
    }

    long long total = listed + unlisted;
    if (total == 0) {
        return {0, 0, 0.0, 0.0};
    }

    return {
        listed,
        unlisted,
        100.0 * listed / total,
        100.0 * unlisted / total
    };
}
