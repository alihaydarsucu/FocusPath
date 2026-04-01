#include "AttentionAnalyzer.hpp"
#include "LogParser.hpp"

#include <QDebug>
#include <algorithm>
#include <string>

AttentionAnalyzer::AttentionAnalyzer(
    const std::vector<WindowEvent>& events,
    const std::vector<std::string>& trackedApps
)
    : events_(events),
      trackedAppsSet(trackedApps.begin(), trackedApps.end())
{
}

AttentionAnalyzer::AttentionStats AttentionAnalyzer::analyze() const {
    if (events_.empty()) {
        qWarning() << "[AttentionAnalyzer] No events to analyze";
        return {};
    }

    long long currentFocusTime = 0;
    long long currentDistractionTime = 0;
    long long maxFocus = 0;
    long long maxDistraction = 0;
    int distractionCount = 0;

    bool previousFocus =
        trackedAppsSet.find(events_.front().windowClass) != trackedAppsSet.end();

    for (const auto& e : events_) {
        bool currentFocus =
            trackedAppsSet.find(e.windowClass) != trackedAppsSet.end();

        // transition detected → finalize previous streak
        if (currentFocus != previousFocus) {
            if (previousFocus) {
                maxFocus = std::max(maxFocus, currentFocusTime);
                currentFocusTime = 0;
            } else {
                if (currentDistractionTime >= MIN_DISTRACTION_MS) {
                    distractionCount++;
                    maxDistraction =
                        std::max(maxDistraction, currentDistractionTime);
                }
                currentDistractionTime = 0;
            }
        }

        // accumulate time AFTER handling transition
        if (currentFocus) {
            currentFocusTime += e.durationMs;
        } else {
            currentDistractionTime += e.durationMs;
        }

        previousFocus = currentFocus;
    }

    // finalize last streak
    if (previousFocus) {
        maxFocus = std::max(maxFocus, currentFocusTime);
    } else if (currentDistractionTime >= MIN_DISTRACTION_MS) {
        distractionCount++;
        maxDistraction =
            std::max(maxDistraction, currentDistractionTime);
    }

    return {
        distractionCount,
        maxDistraction,
        maxFocus
    };
}

