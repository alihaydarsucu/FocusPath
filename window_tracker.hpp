#pragma once

#include <string>
#include <chrono>
#include <fstream>

// X11 TİPLERİNİ SADECE İLERİ TANIMLA
using Window = unsigned long;
using Atom   = unsigned long;

struct _XDisplay;
using Display = _XDisplay;

class WindowTracker {
public:
    explicit WindowTracker(const std::string& logDir);
    ~WindowTracker();

    void tick();

private:
    Window getActiveWindow();
    std::string getWindowTitle(Window w);
    void logWindow(Window w, long ms, std::string title);

    Display* display;
    Window root;
    Atom atomActive;
    Atom atomNetWmName;
    Atom atomUtf8;

    Window current;
    std::string current_title;
    std::chrono::steady_clock::time_point lastSwitch;
    std::ofstream out;
};
