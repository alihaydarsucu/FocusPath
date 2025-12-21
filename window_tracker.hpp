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
        WindowTracker(const std::string& path);
        ~WindowTracker();
        void tick();
    
    private:
        Window getActiveWindow();
        void logWindow(Window w, long ms);

        Display* display;
        Window root;
        Atom atomActive;
        Window current;
        std::chrono::steady_clock::time_point lastSwitch;
        std::ofstream out;
};

#endif //WINDOW_TRACKER_HPP