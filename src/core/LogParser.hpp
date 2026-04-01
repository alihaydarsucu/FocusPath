#ifndef LOG_PARSER_HPP
#define LOG_PARSER_HPP

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>

struct WindowEvent {
    unsigned long windowId;
    std::string windowClass;
    std::string windowTitle;
    long long durationMs;
    std::time_t endedAt;
};

class LogParser {
private:
    std::vector<WindowEvent> eventLog;
    std::unordered_map<std::string, long long> totals;

    void parseFile(const std::string& file);
    WindowEvent parseEvent(const nlohmann::json& j) const;
    bool isValid(const nlohmann::json& j) const;
    
public:
    explicit LogParser(const std::string& file);

    const std::unordered_map<std::string, long long>& totalsByApp() const;
    const std::vector<WindowEvent>& events() const;

};

#endif // LOG_PARSER_HPP
