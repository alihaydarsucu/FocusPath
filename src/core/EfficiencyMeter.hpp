#ifndef EFFICIENCY_METER_HPP
#define EFFICIENCY_METER_HPP

#include "LogParser.hpp"
#include "AppUsageAnalyzer.hpp"
#include "AttentionAnalyzer.hpp"
#include <string>
#include <vector>

class EfficiencyMeter {
public:
    EfficiencyMeter(
        const std::string& logFile,
        std::vector<std::string> trackedApps
    );

    void analyze();

    const AppUsageAnalyzer::AppUsageResult& appUsage() const;
    const AttentionAnalyzer::AttentionStats& attention() const;


    // Re-exposed parser data (read-only)
    const std::unordered_map<std::string, long long>&
    totalsByApp() const;

    const std::vector<WindowEvent>&
    events() const;

private:
    // Inputs
    std::vector<std::string> trackedApps;

    // Core components
    LogParser parser;

    // Results
    AppUsageAnalyzer::AppUsageResult appUsageResult{};
    AttentionAnalyzer::AttentionStats attentionStats{};
};

#endif // EFFICIENCY_METER_HPP