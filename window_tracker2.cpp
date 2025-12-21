#include "window_tracker2.hpp"
using json = nlohmann::json;

WindowTracker::WindowTracker(const std::string& path)
    : out(path, std::ios::app)
{
    display = XOpenDisplay(NULL);
    root = DefaultRootWindow(display);
    atomActive = XInternAtom(display, "_NET_ACTIVE_WINDOW", True);

    lastSwitch = std::chrono::steady_clock::now();
    current = getActiveWindow();
}

WindowTracker::~WindowTracker() {
    XCloseDisplay(display);
}

void WindowTracker::tick() {
    Window w = getActiveWindow();

    XWindowAttributes attr;
    if (!XGetWindowAttributes(display, w, &attr) || attr.map_state != IsViewable) {
        return;
    }

    if (w != current) {
        auto now = std::chrono::steady_clock::now();
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSwitch).count();

        logWindow(current, ms);

        current = w;
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

void WindowTracker::logWindow(Window w, long ms) {
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
    {"duration_ms", ms},
    {"ended_at", std::time(nullptr)}
    };


    out << entry.dump() << "\n";
    out.flush();
}
