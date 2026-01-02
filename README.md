# FocusPath

<p align="center">
  <img src="pictures/FocusPath.png" alt="FocusPath logo" width="200" />
</p>

Measure how focused you stay by comparing the apps you intended to use with the ones you actually used. FocusPath records every active window change, highlights distraction streaks, and renders SVG charts so you can review your sessions after they end.

## Features
- Workflow planning: pick whitelisted apps, set a duration, and label the session with an emoji.
- Live tracking on X11: the active window and title are logged every 200 ms whenever focus or title changes.
- Analysis pipeline: parses JSONL window logs, measures time on allowed vs unlisted apps, and detects distraction streaks.
- Reporting: generates SVG charts (usage table, timeline, pie, attention box) and displays them in-app; files are saved for later.
- History and templates: reuse prior workflows or start from saved templates.

## Requirements
- Linux with X11 (window tracking uses X11 APIs).
- Qt 5.15+ or Qt 6 (Widgets, Svg, SvgWidgets modules).
- CMake 3.16+ and a C++17 compiler.
- X11 development headers.
- nlohmann-json single-header library available to the compiler (included by your system package or placed in include paths).

## Build and Run
```bash
git clone <repo-url>
cd fp_project1
cmake -S . -B build
cmake --build build
./build/fp_project1
```
If Qt is not in your default CMake paths, pass `-DCMAKE_PREFIX_PATH=/path/to/Qt` when running the first cmake command.

## Using FocusPath
1) Start the app. The `logs/` folder is created beside the binary.
2) Go to **Workflow Setup**:
   - Search and add the apps you want to whitelist (allowed focus apps).
   - Move to **Duration Setup**, name the workflow, choose an emoji, and set the session length.
3) Click **Start Tracking**. The Workflow page appears while the tracker writes JSONL events.
4) When you complete the workflow, the Result page opens automatically with charts. You can also revisit past runs via **History** or **Templates**.

## Where Data Is Stored
- Window logs: `logs/window_log_YYYY-MM-DD_HH-MM.jsonl` (JSON Lines). Example:
  ```json
  {"window_id": 123456, "window_class": "code", "window_title": "main.cpp – FocusPath", "duration_ms": 8421, "ended_at": 1735729200}
  ```
- Generated charts: `results/<workflow_date>_*.svg` (usage table, timeline, pie, attention stats).
- Saved workflows/templates: `workflows/*.json` (created via the UI pages).

## How It Works (brief)
- `SystemController` drives a `SystemHandler` ticking every 200 ms, which delegates to `WindowTracker` to capture the active X11 window and title changes.
- `WindowTracker` appends JSONL entries whenever focus or title changes.
- `EfficiencyMeter` reads a selected log file, aggregates usage per window class, and runs `AppUsageAnalyzer` and `AttentionAnalyzer` to compute focus vs distraction metrics.
- `GraphicGenerator` renders charts consumed by the Result page.

## Troubleshooting
- No charts appear: ensure the workflow date matches a log file; the app falls back to the nearest log but needs at least one entry.
- Empty log files: confirm you are on X11 (not Wayland) and that the app has permission to read window info.
- Qt not found: pass `-DCMAKE_PREFIX_PATH` to CMake or source your Qt environment script.

## License
This project is licensed under the terms of the LICENSE file in the repository.

## Contributors
- Ali Haydar Sucu
- Alpdoğan Kurt
- Fatih Şenal
- Burak Semih Bileci
