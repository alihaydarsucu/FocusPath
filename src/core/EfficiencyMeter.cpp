#include "EfficiencyMeter.hpp"

EfficiencyMeter::EfficiencyMeter(
    const std::string& logFile,
    std::vector<std::string> trackedApps
)
    : trackedApps(std::move(trackedApps)),
      parser(logFile)
{
}

void EfficiencyMeter::analyze() {
    // App usage
    AppUsageAnalyzer usageAnalyzer(
        parser.totalsByApp(),
        trackedApps
    );
    appUsageResult = usageAnalyzer.analyze();

    // Attention / distraction
    AttentionAnalyzer attentionAnalyzer(
        parser.events(),
        trackedApps
    );
    attentionStats = attentionAnalyzer.analyze();
}

const AppUsageAnalyzer::AppUsageResult&
EfficiencyMeter::appUsage() const {
    return appUsageResult;
}

const AttentionAnalyzer::AttentionStats&
EfficiencyMeter::attention() const {
    return attentionStats;
}

const std::unordered_map<std::string, long long>&
EfficiencyMeter::totalsByApp() const {
    return parser.totalsByApp();
}

const std::vector<WindowEvent>&
EfficiencyMeter::events() const {
    return parser.events();
}