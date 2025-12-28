#ifndef WINDOW_TRACKER_HPP
#define WINDOW_TRACKER_HPP

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>

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

#endif //WINDOW_TRACKER_HPP