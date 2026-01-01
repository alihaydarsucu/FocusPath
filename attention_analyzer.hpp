#ifndef ATTENTION_ANALYZER_HPP
#define ATTENTION_ANALYZER_HPP

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include "log_parser.hpp"


class AttentionAnalyzer {
public:

    struct AttentionStats {
    int distraction_count = 0;
    long long longest_distraction_ms = 0;
    long long longest_focus__ms = 0;
    };

    AttentionAnalyzer(
        const std::vector<WindowEvent>& events,
        const std::vector<std::string>& trackedApps
    );

    AttentionStats analyze() const;

private:
    const std::vector<WindowEvent>& events_;
    std::unordered_set<std::string> trackedAppsSet;
    static constexpr long long MIN_DISTRACTION_MS = 1000;
};


#endif // ATTENTION_ANALYZER_HPP