#ifndef ATTENTION_ANALYZER_HPP
#define ATTENTION_ANALYZER_HPP

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include "LogParser.hpp"


class AttentionAnalyzer {
public:

    struct AttentionStats {
        int distractionCount = 0;
        long long longestDistractionMs = 0;
        long long longestFocusMs = 0;
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