#ifndef APP_USAGE_ANALYZER_HPP
#define APP_USAGE_ANALYZER_HPP

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

class AppUsageAnalyzer {
public:
    using UsageMap = std::unordered_map<std::string, long long>;

    struct AppUsageResult {
        long long listedTime;
        long long unlistedTime;
        double listedPercent;
        double unlistedPercent;
    };

    AppUsageAnalyzer(
        const UsageMap& usage,
        const std::vector<std::string>& trackedApps
    );

    AppUsageResult analyze() const;

private:
    const UsageMap& usage_;
    std::unordered_set<std::string> trackedApps_;
};

#endif // APP_USAGE_ANALYZER_HPP