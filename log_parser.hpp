#pragma once

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>

struct WindowEvent {
    unsigned long window_id;
    std::string window_class;
    long long duration_ms;
    std::time_t ended_at;
};

class WindowUsageParser {
private:
    std::vector<WindowEvent> event_log;
    std::unordered_map<std::string, long long> totals;

    void parse_file(const std::string& file);
    WindowEvent parse_event(const nlohmann::json& j) const;
    bool is_valid(const nlohmann::json& j) const;
    
public:
    explicit WindowUsageParser(const std::string& file);

    const std::unordered_map<std::string, long long>& totals_by_app() const;
    const std::vector<WindowEvent>& events() const;

};
