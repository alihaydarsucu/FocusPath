#include "window_tracker.hpp"
#include <string>
using json = nlohmann::json;

std::string makeSessionFilename(const std::string& baseDir) {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    localtime_r(&now, &tm);

    char buf[64];
    std::strftime(buf, sizeof(buf),
                  "window_log_%Y-%m-%d_%H-%M.jsonl", &tm);

    return baseDir + "/" + buf;
}

WindowTracker::WindowTracker(const std::string& logDir)
{
    std::string filename = makeSessionFilename(logDir);
    out.open(filename, std::ios::out | std::ios::app);

    display = XOpenDisplay(NULL);
    root = DefaultRootWindow(display);
    atomActive = XInternAtom(display, "_NET_ACTIVE_WINDOW", True);
    atomNetWmName = XInternAtom(display, "_NET_WM_NAME", False);
    atomUtf8 = XInternAtom(display, "UTF8_STRING", False);

    lastSwitch = std::chrono::steady_clock::now();
    current = getActiveWindow();
    current_title = getWindowTitle(current);
}


WindowTracker::~WindowTracker() {
    XCloseDisplay(display);
}


void WindowTracker::tick() {
    Window w = getActiveWindow();
    if (w == 0) return;

    XWindowAttributes attr;
    if (!XGetWindowAttributes(display, w, &attr) || attr.map_state != IsViewable) {
        return;
    }

    auto now = std::chrono::steady_clock::now();

    // 1. Focus change
    if (w != current) {
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastSwitch
        ).count();

        logWindow(current, ms, current_title);

        current = w;
        current_title = getWindowTitle(current);
        lastSwitch = now;
        return;
    }

    // 2. Title change (focused window only)
    std::string title = getWindowTitle(current);
    if (title != current_title) {
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastSwitch
        ).count();

        logWindow(current, ms, current_title);

        current_title = title;
        lastSwitch = now;
    }
}


Window WindowTracker::getActiveWindow() {
    Atom actualType;
    int actualFormat;
    unsigned long nItems, bytesAfter;
    unsigned char* prop = nullptr;

    XGetWindowProperty(
        display,
        root,
        atomActive,
        0, ~0,
        False,
        AnyPropertyType,
        &actualType, &actualFormat,
        &nItems, &bytesAfter,
        &prop
    );

    if (nItems == 0) return 0;

    Window w = *(Window*)prop;
    XFree(prop);
    return w;
}

std::string WindowTracker::getWindowTitle(Window w) {

    Atom actualType;
    int actualFormat;
    unsigned long nItems, bytesAfter;
    unsigned char* prop = nullptr;

    // Try _NET_WM_NAME first
    if (XGetWindowProperty(
            display,
            w,
            atomNetWmName,
            0, ~0,
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

    // Fallback to WM_NAME
    char* name = nullptr;
    if (XFetchName(display, w, &name) && name) {
        std::string title(name);
        XFree(name);
        return title;
    }

    return "unknown";
}


void WindowTracker::logWindow(Window w, long ms, std::string title) {
    if (w == 0) return;

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

    std::string windowName = name ? name : "unknown";
    if (name) XFree(name);

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
