#include "WindowTracker.hpp"

#include <nlohmann/json.hpp>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <ctime>
#include <stdexcept>
#include <iostream>
using json = nlohmann::json;

/* --------------------------------------------------
 * Helpers
 * -------------------------------------------------- */
std::string makeSessionFilename(const std::string& baseDir) {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    localtime_r(&now, &tm);

    char buf[64];
    std::strftime(buf, sizeof(buf),
                  "window_log_%Y-%m-%d_%H-%M.jsonl", &tm);

    return baseDir + "/" + buf;
}

/* --------------------------------------------------
 * Constructor / Destructor
 * -------------------------------------------------- */
WindowTracker::WindowTracker(const std::string& logDir)
{
    std::string filename = makeSessionFilename(logDir);
    out.open(filename, std::ios::out | std::ios::app);

    display = XOpenDisplay(nullptr);
    if (!display) {
        throw std::runtime_error("XOpenDisplay failed");
    }

    // ❗ X11 crash etmesin diye global error handler
    XSetErrorHandler([](Display*, XErrorEvent*) {
        return 0;
    });

    root = DefaultRootWindow(display);

    atomActive    = XInternAtom(display, "_NET_ACTIVE_WINDOW", True);
    atomNetWmName = XInternAtom(display, "_NET_WM_NAME", False);
    atomUtf8      = XInternAtom(display, "UTF8_STRING", False);

    lastSwitch = std::chrono::steady_clock::now();

    current = getActiveWindow();
    if (current != 0)
        currentTitle = getWindowTitle(current);
    else
        currentTitle = "unknown";
}

WindowTracker::~WindowTracker() {
    if (display)
        XCloseDisplay(display);
}

/* --------------------------------------------------
 * Main tick
 * -------------------------------------------------- */
void WindowTracker::tick() {
    if (!display) return;

    Window w = getActiveWindow();
    if (w == 0) return;

    XWindowAttributes attr;
    if (!XGetWindowAttributes(display, w, &attr))
        return;

    if (attr.map_state != IsViewable)
        return;

    auto now = std::chrono::steady_clock::now();
    // 1️ Focus change
    if (w != current) {
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now - lastSwitch
                      ).count();

        logWindow(current, ms, currentTitle);
        current = w;
        currentTitle = getWindowTitle(current);
        lastSwitch = now;
        return;
    }

    // 2️ Title change
    std::string title = getWindowTitle(current);
    if (title != currentTitle) {
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now - lastSwitch
                      ).count();

        logWindow(current, ms, currentTitle);

        currentTitle = title;
        lastSwitch = now;
    }
}

/* --------------------------------------------------
 * Active window
 * -------------------------------------------------- */
Window WindowTracker::getActiveWindow() {
    if (!display) return 0;

    Atom actualType;
    int actualFormat;
    unsigned long nItems = 0, bytesAfter = 0;
    unsigned char* prop = nullptr;

    int status = XGetWindowProperty(
        display,
        root,
        atomActive,
        0, 1,              // ✅ SADECE 1 WINDOW
        False,
        AnyPropertyType,
        &actualType,
        &actualFormat,
        &nItems,
        &bytesAfter,
        &prop
        );

    if (status != Success || nItems == 0 || !prop)
        return 0;

    Window w = *(Window*)prop;
    XFree(prop);
    return w;
}

/* --------------------------------------------------
 * Window title
 * -------------------------------------------------- */
std::string WindowTracker::getWindowTitle(Window w) {
    if (!display || w == 0)
        return "unknown";

    Atom actualType;
    int actualFormat;
    unsigned long nItems, bytesAfter;
    unsigned char* prop = nullptr;

    // Try UTF-8 title
    if (XGetWindowProperty(
            display,
            w,
            atomNetWmName,
            0, 1024,
            False,
            atomUtf8,
            &actualType,
            &actualFormat,
            &nItems,
            &bytesAfter,
            &prop
            ) == Success && prop)
    {
        std::string title(reinterpret_cast<char*>(prop));
        XFree(prop);
        return title;
    }

    // Fallback WM_NAME
    char* name = nullptr;
    if (XFetchName(display, w, &name) && name) {
        std::string title(name);
        XFree(name);
        return title;
    }

    return "unknown";
}

/* --------------------------------------------------
 * Logging
 * -------------------------------------------------- */
void WindowTracker::logWindow(Window w, long ms, std::string title) {

    if (!display || w == 0 || ms <= 0)
        return;

    char* name = nullptr;
    XFetchName(display, w, &name);

    XClassHint classHint;
    std::string cls = "unknown";

    if (XGetClassHint(display, w, &classHint)) {
        if (classHint.res_class)
            cls = classHint.res_class;

        if (classHint.res_name)
            XFree(classHint.res_name);
        if (classHint.res_class)
            XFree(classHint.res_class);
    }

    if (name)
        XFree(name);

    json entry = {
        {"window_id", static_cast<unsigned long>(w)},
        {"window_class", cls},
        {"window_title", title},
        {"duration_ms", ms},
        {"ended_at", std::time(nullptr)}
    };

    out << entry.dump() << "\n";
    out.flush();
}
