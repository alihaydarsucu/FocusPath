#include "log_parser.hpp"
#include <stdexcept>

using json = nlohmann::json;

WindowUsageParser::WindowUsageParser(const std::string& file) {
    std::ifstream test(file);
    if (!test) {
        throw std::runtime_error("Cannot open file");
    }
    parse_file(file);
}

const std::vector<WindowEvent>&
WindowUsageParser::events() const {
    return event_log;
}

const std::unordered_map<std::string, long long>&
WindowUsageParser::totals_by_app() const {
    return totals;
}

void WindowUsageParser::parse_file(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        json j = json::parse(line);

        if (!is_valid(j))
            continue;

        WindowEvent e = parse_event(j);
        event_log.push_back(e);
        totals[e.window_class] += e.duration_ms;
    }
}


WindowEvent
WindowUsageParser::parse_event(const json& j) const {
    return {
        j.at("window_id").get<unsigned long>(),
        j.at("window_class").get<std::string>(),
        j.at("duration_ms").get<long long>(),
        j.at("ended_at").get<std::time_t>()
    };
}

bool WindowUsageParser::is_valid(const json& j) const {
    return j.contains("window_id") &&
           j.contains("window_class") &&
           j.contains("duration_ms") &&
           j.contains("ended_at");
}
