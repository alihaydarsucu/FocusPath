#include "LogParser.hpp"
#include <stdexcept>

using json = nlohmann::json;

LogParser::LogParser(const std::string& file) {
    std::ifstream test(file);
    if (!test) {
        throw std::runtime_error("Cannot open file");
    }
    parseFile(file);
}

const std::vector<WindowEvent>&
LogParser::events() const {
    return eventLog;
}

const std::unordered_map<std::string, long long>&
LogParser::totalsByApp() const {
    return totals;
}

void LogParser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        json j = json::parse(line);

        if (!isValid(j))
            continue;

        WindowEvent e = parseEvent(j);
        eventLog.push_back(e);
        totals[e.windowClass] += e.durationMs;
    }
}


WindowEvent
LogParser::parseEvent(const json& j) const {
    return {
        j.at("window_id").get<unsigned long>(),
        j.at("window_class").get<std::string>(),
        j.at("window_title").get<std::string>(),
        j.at("duration_ms").get<long long>(),
        j.at("ended_at").get<std::time_t>()
    };
}

bool LogParser::isValid(const json& j) const {
    return j.contains("window_id") &&
           j.contains("window_class") &&
           j.contains("window_title") &&
           j.contains("duration_ms") &&
           j.contains("ended_at");
}
