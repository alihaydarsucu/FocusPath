<p align="center">
  <img src="pictures/FocusPath.png" alt="FocusPath logo" width="350" />
</p>

# FocusPath

FocusPath is a desktop utility designed to help you track your productivity by comparing your intended application usage with your actual focus. It monitors active window transitions on X11, identifies distraction streaks, and generates detailed SVG-based visual reports.

🎥 **Watch the presentation:** [YouTube Link](https://youtu.be/2Ad7iqPG2-s)

---

### Key Capabilities
- **Workflow Planning:** Whitelist specific applications for a session, set your target duration, and tag it with an emoji.
- **Real-time X11 Tracking:** Monitors focus changes and window titles every 200ms to build an accurate activity log.
- **Data Analysis:** Process raw JSONL logs to calculate time spent on productive vs. unproductive tasks.
- **Visual Reporting:** Automatically generates SVG charts (Timeline, Pie Chart, Usage Table) for every session.
- **History & Templates:** Save your favorite setups as templates and review past productivity trends.

### Prerequisites
- **OS:** Linux with X11 (Wayland is not currently supported for window tracking).
- **Framework:** Qt 5.15+ or Qt 6 (Widgets and SVG modules).
- **Build Tools:** CMake 3.16+ and a C++17 compatible compiler.
- **Dependencies:** X11 development headers and `nlohmann-json`.

### Build & Installation
```bash
git clone <repo-url>
cd FocusPath
cmake -S . -B build
cmake --build build
./build/fp_project1
```
*Note: If Qt is installed in a non-standard location, use `-DCMAKE_PREFIX_PATH` during the configuration step.*

### How to Use
1. **Initialize:** Run the application. It will create `logs/` and `workflows/` directories automatically.
2. **Setup:** Open the **Workflow Setup** page, search for the apps you want to use, and finalize your session parameters (name, emoji, time).
3. **Focus:** Click **Start Tracking**. The app will minimize/background and begin logging your X11 activity.
4. **Review:** Once the timer ends, the **Result** page will display your performance charts.

### Data & Architecture
- **Logs:** Saved as `.jsonl` files in `logs/`. Each entry includes window ID, class, title, and duration.
- **Charts:** SVG files are stored in `results/` for later review.
- **Engine:**
    - `SystemController` & `SystemHandler`: Orchestrates the 200ms tick loop.
    - `WindowTracker`: Direct interface with X11 for focus capture.
    - `EfficiencyMeter`: High-level orchestrator for the analysis pipeline.
    - `LogParser` & `AttentionAnalyzer`: Core logic for turning raw logs into metrics.

### Troubleshooting
- **Missing Charts:** Ensure your workflow timeframe matches the generated log files.
- **No Data in Logs:** Verify you are running an X11 session. Wayland environments do not allow standard X11 focus tracking.
- **Build Errors:** Ensure all Qt SVG modules are installed (e.g., `libqt5svg5-dev` or `qt6-svg-dev`).

### License
Distributed under the terms found in the [LICENSE](LICENSE) file.
