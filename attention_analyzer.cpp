#include "attention_analyzer.hpp"
#include "log_parser.hpp"

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

    long long current_focus_time = 0;
    long long current_distraction_time = 0;
    long long max_focus = 0;
    long long max_distraction = 0;
    int distraction_count = 0;

    bool previousFocus =
        trackedAppsSet.find(events_.front().window_class) != trackedAppsSet.end();

    for (const auto& e : events_) {
        bool currentFocus =
            trackedAppsSet.find(e.window_class) != trackedAppsSet.end();

        // transition detected → finalize previous streak
        if (currentFocus != previousFocus) {
            if (previousFocus) {
                max_focus = std::max(max_focus, current_focus_time);
                current_focus_time = 0;
            } else {
                if (current_distraction_time >= MIN_DISTRACTION_MS) {
                    distraction_count++;
                    max_distraction =
                        std::max(max_distraction, current_distraction_time);
                }
                current_distraction_time = 0;
            }
        }

        // accumulate time AFTER handling transition
        if (currentFocus) {
            current_focus_time += e.duration_ms;
        } else {
            current_distraction_time += e.duration_ms;
        }

        previousFocus = currentFocus;
    }

    // finalize last streak
    if (previousFocus) {
        max_focus = std::max(max_focus, current_focus_time);
    } else if (current_distraction_time >= MIN_DISTRACTION_MS) {
        distraction_count++;
        max_distraction =
            std::max(max_distraction, current_distraction_time);
    }

    return {
        distraction_count,
        max_distraction,
        max_focus
    };
}

