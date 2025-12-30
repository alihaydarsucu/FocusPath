#ifndef GRAPHIC_GENERATOR_H
#define GRAPHIC_GENERATOR_H

#include <string>
#include <map>
#include <vector>

// ===== STRUCT TANIMLARI =====

// Tek bir olay
struct AppEvent {
    std::string appName;
    long duration;  // ms
};

// EfficiencyMeter'dan gelen veri
struct SessionData {
    std::map<std::string, long> totalUsage;
    std::vector<AppEvent> events;
    
    long listedAppsTime;
    double listedAppsPercent;
    long unlistedAppsTime;
    double unlistedAppsPercent;
    
    int distractionCount;
    long longestDistraction;
    long longestFocus;
};

// Tek bir grafik
struct ChartOutput {
    std::string svgContent;
    std::string title;
    std::string chartType;
};

// 4 grafik birden
struct SessionCharts {
    ChartOutput usageTable;
    ChartOutput timeline;
    ChartOutput pieChart;
    ChartOutput statsBox;
};

// ===== SINIF TANIMI =====

class GraphicGenerator {
public:
    GraphicGenerator();
    
    SessionCharts generateCharts(const SessionData& data);
    void setDarkMode(bool enable);

private:
    bool darkMode;
    
    ChartOutput createUsageTable(const std::map<std::string, long>& totalUsage);
    ChartOutput createTimeline(const std::vector<AppEvent>& events);
    ChartOutput createPieChart(long listedTime, double listedPercent, 
                               long unlistedTime, double unlistedPercent);
    ChartOutput createStatsBox(int distractionCount, long longestDistraction, 
                               long longestFocus);
    
    std::string getColor(int index);
    std::string formatTime(long ms);
    std::string formatSeconds(long seconds);
};

#endif
