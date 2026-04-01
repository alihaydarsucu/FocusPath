#include "GraphicGeneratorStructs.h"

#include <QDebug>
#include <cmath>
#include <iomanip>
#include <sstream>

GraphicGenerator::GraphicGenerator() : darkMode(false) {}

void GraphicGenerator::setDarkMode(bool enable) {
    darkMode = enable;
    qDebug() << "[GraphicGenerator] Dark mode set to" << enable;
}

std::string GraphicGenerator::getColor(int index) {
    const std::string colors[] = {
        "rgb(54,162,235)", "rgb(255,99,132)", "rgb(75,192,192)", 
        "rgb(255,159,64)", "rgb(153,102,255)", "rgb(255,205,86)",
        "rgb(201,203,207)", "rgb(99,179,237)"
    };
    return colors[index % 8];
}

std::string GraphicGenerator::formatTime(long ms) {
    long seconds = ms / 1000;
    long minutes = seconds / 60;
    long hours = minutes / 60;
    
    if (hours > 0) {
        return std::to_string(hours) + "h " + std::to_string(minutes % 60) + "m";
    } else if (minutes > 0) {
        return std::to_string(minutes) + "m " + std::to_string(seconds % 60) + "s";
    } else {
        return std::to_string(seconds) + "s";
    }
}

std::string GraphicGenerator::formatSeconds(long seconds) {
    if (seconds >= 60) {
        return std::to_string(seconds / 60) + "m " + std::to_string(seconds % 60) + "s";
    }
    return std::to_string(seconds) + "s";
}

SessionCharts GraphicGenerator::generateCharts(const SessionData& data) {
    qDebug() << "[GraphicGenerator] Generating charts with" << data.totalUsage.size()
             << "apps and" << data.events.size() << "events"
             << "listed time" << data.listedAppsTime
             << "unlisted time" << data.unlistedAppsTime;

    SessionCharts charts;
    
    charts.usageTable = createUsageTable(data.totalUsage);
    charts.timeline = createTimeline(data.events);
    charts.pieChart = createPieChart(data.listedAppsTime, data.listedAppsPercent,
                                     data.unlistedAppsTime, data.unlistedAppsPercent);
    charts.statsBox = createStatsBox(data.distractionCount, data.longestDistraction, 
                                     data.longestFocus);
    
    return charts;
}

ChartOutput GraphicGenerator::createUsageTable(const std::map<std::string, long>& totalUsage) {
    qDebug() << "[GraphicGenerator] Building usage table rows:" << totalUsage.size();

    ChartOutput output;
    output.title = "Application Usage Table";
    output.chartType = "table";
    
    std::stringstream svg;
    svg << "<svg width=\"900\" height=\"" << (80 + totalUsage.size() * 40) 
        << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Title
    svg << "<text x=\"450\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#0288D1\">Total Usage Duration</text>\n";
    
    // Table headers
    svg << "<rect x=\"50\" y=\"50\" width=\"800\" height=\"40\" fill=\"#0288D1\" "
        << "stroke=\"#E8F0F7\" stroke-width=\"2\"/>\n";
    svg << "<text x=\"250\" y=\"75\" text-anchor=\"middle\" font-size=\"16\" "
        << "font-weight=\"bold\" fill=\"white\">Application</text>\n";
    svg << "<text x=\"750\" y=\"75\" text-anchor=\"middle\" font-size=\"16\" "
        << "font-weight=\"bold\" fill=\"white\">Duration</text>\n";
    
    // Rows
    int y = 90;
    int index = 0;
    for (const auto& pair : totalUsage) {
        std::string rowColor = (index % 2 == 0) ? "#F8FBFD" : "#FFFFFF";
        
        svg << "<rect x=\"50\" y=\"" << y << "\" width=\"800\" height=\"40\" "
            << "fill=\"" << rowColor << "\" stroke=\"#E8F0F7\" stroke-width=\"1\"/>\n";
        
        svg << "<text x=\"250\" y=\"" << (y + 25) << "\" text-anchor=\"middle\" "
            << "font-size=\"14\" fill=\"#333\">" << pair.first << "</text>\n";
        
        svg << "<text x=\"750\" y=\"" << (y + 25) << "\" text-anchor=\"middle\" "
            << "font-size=\"14\" font-weight=\"bold\" fill=\"#0288D1\">" 
            << formatTime(pair.second) << "</text>\n";
        
        y += 40;
        index++;
    }
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}

ChartOutput GraphicGenerator::createTimeline(const std::vector<AppEvent>& events) {
    qDebug() << "[GraphicGenerator] Creating timeline for" << events.size() << "events";

    ChartOutput output;
    output.title = "Activity Timeline";
    output.chartType = "timeline";
    
    // Calculate total time
    long totalTime = 0;
    for (const auto& event : events) {
        totalTime += event.duration;
    }

    if (totalTime <= 0) {
        qDebug() << "[GraphicGenerator] Timeline has no duration; emitting empty chart";
        std::stringstream emptySvg;
        emptySvg << "<svg width=\"900\" height=\"120\" xmlns=\"http://www.w3.org/2000/svg\">";
        emptySvg << "<text x=\"450\" y=\"60\" text-anchor=\"middle\" font-size=\"14\" fill=\"#666\">";
        emptySvg << "No events to display";
        emptySvg << "</text></svg>";
        output.svgContent = emptySvg.str();
        return output;
    }
    
    std::stringstream svg;
    svg << "<svg width=\"900\" height=\"250\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Title
    svg << "<text x=\"450\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#0288D1\">Application Timeline</text>\n";
    
    // Timeline line
    double timelineWidth = 800;
    double currentX = 50;
    int colorIndex = 0;
    
    for (const auto& event : events) {
        double width = (double)event.duration / totalTime * timelineWidth;
        
        // Segment
        svg << "<rect x=\"" << currentX << "\" y=\"80\" width=\"" << width 
            << "\" height=\"60\" fill=\"" << getColor(colorIndex) 
            << "\" stroke=\"white\" stroke-width=\"2\"/>\n";
        
        // Label (if wide enough)
        if (width > 40) {
            // Shorten name (first 8 chars)
            std::string shortName = event.appName.length() > 8 
                                   ? event.appName.substr(0, 8) 
                                   : event.appName;
            
            svg << "<text x=\"" << (currentX + width/2) << "\" y=\"110\" "
                << "text-anchor=\"middle\" font-size=\"12\" font-weight=\"bold\" "
                << "fill=\"white\">" << shortName << "</text>\n";
            
            svg << "<text x=\"" << (currentX + width/2) << "\" y=\"125\" "
                << "text-anchor=\"middle\" font-size=\"10\" fill=\"white\">" 
                << formatTime(event.duration) << "</text>\n";
        }
        
        currentX += width;
        colorIndex++;
    }
    
    // Time axis
    svg << "<line x1=\"50\" y1=\"160\" x2=\"850\" y2=\"160\" "
        << "stroke=\"#0288D1\" stroke-width=\"2\"/>\n";
    
    svg << "<text x=\"50\" y=\"180\" font-size=\"12\" fill=\"#666\">0s</text>\n";
    svg << "<text x=\"850\" y=\"180\" text-anchor=\"end\" font-size=\"12\" "
        << "fill=\"#666\">" << formatTime(totalTime) << "</text>\n";
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}

ChartOutput GraphicGenerator::createPieChart(long listedTime, double listedPercent, 
                                             long unlistedTime, double unlistedPercent) {
    qDebug() << "[GraphicGenerator] Creating pie chart" << listedPercent << unlistedPercent;

    ChartOutput output;
    output.title = "Listed/Unlisted Applications";
    output.chartType = "pie";
    
    std::stringstream svg;
    svg << "<svg width=\"900\" height=\"500\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Title
    svg << "<text x=\"450\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#0288D1\">Application Distribution</text>\n";
    
    double cx = 250, cy = 250, r = 120;
    
    // Special case: 100% Listed
    if (listedPercent >= 99.9) {
        svg << "<circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << r 
            << "\" fill=\"rgb(75,192,192)\" stroke=\"white\" stroke-width=\"3\"/>\n";
        svg << "<text x=\"" << cx << "\" y=\"" << cy + 5 
            << "\" text-anchor=\"middle\" font-size=\"16\" font-weight=\"bold\" "
            << "fill=\"white\">100%</text>\n";
    }
    // Special case: 100% Unlisted
    else if (unlistedPercent >= 99.9) {
        svg << "<circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << r 
            << "\" fill=\"rgb(255,99,132)\" stroke=\"white\" stroke-width=\"3\"/>\n";
        svg << "<text x=\"" << cx << "\" y=\"" << cy + 5 
            << "\" text-anchor=\"middle\" font-size=\"16\" font-weight=\"bold\" "
            << "fill=\"white\">100%</text>\n";
    }
    // Normal case: Both slices present
    else {
        // Listed apps (teal)
        double listedAngle = listedPercent * 3.6;  // Convert percentage to angle
        double listedRad = listedAngle * M_PI / 180.0;
    
        
        double x1 = cx + r;
        double y1 = cy;
        double x2 = cx + r * cos(listedRad);
        double y2 = cy + r * sin(listedRad);
        
        svg << "<path d=\"M " << cx << " " << cy 
            << " L " << x1 << " " << y1
            << " A " << r << " " << r << " 0 " << (listedAngle > 180 ? 1 : 0) << " 1 "
            << x2 << " " << y2 << " Z\" "
            << "fill=\"rgb(75,192,192)\" stroke=\"white\" stroke-width=\"3\"/>\n";
        
        // Listed label
        double listedMidAngle = listedAngle / 2 * M_PI / 180.0;
        double listedLabelX = cx + r * 0.6 * cos(listedMidAngle);
        double listedLabelY = cy + r * 0.6 * sin(listedMidAngle);
        
        svg << "<text x=\"" << listedLabelX << "\" y=\"" << listedLabelY 
            << "\" text-anchor=\"middle\" font-size=\"16\" font-weight=\"bold\" "
            << "fill=\"white\">" << std::fixed << std::setprecision(1) 
            << listedPercent << "%</text>\n";
        
        // Unlisted apps (red)
        double unlistedAngle = unlistedPercent * 3.6;
        double unlistedRad = (listedAngle + unlistedAngle) * M_PI / 180.0;
        
        double x3 = cx + r * cos(unlistedRad);
        double y3 = cy + r * sin(unlistedRad);
        
        svg << "<path d=\"M " << cx << " " << cy 
            << " L " << x2 << " " << y2
            << " A " << r << " " << r << " 0 " << (unlistedAngle > 180 ? 1 : 0) << " 1 "
            << x3 << " " << y3 << " Z\" "
            << "fill=\"rgb(255,99,132)\" stroke=\"white\" stroke-width=\"3\"/>\n";
        
        // Unlisted label
        double unlistedMidAngle = (listedAngle + unlistedAngle/2) * M_PI / 180.0;
        double unlistedLabelX = cx + r * 0.6 * cos(unlistedMidAngle);
        double unlistedLabelY = cy + r * 0.6 * sin(unlistedMidAngle);
        
        svg << "<text x=\"" << unlistedLabelX << "\" y=\"" << unlistedLabelY 
            << "\" text-anchor=\"middle\" font-size=\"16\" font-weight=\"bold\" "
            << "fill=\"white\">" << std::fixed << std::setprecision(1) 
            << unlistedPercent << "%</text>\n";
    }
    
    // Legend
    svg << "<rect x=\"550\" y=\"150\" width=\"25\" height=\"25\" fill=\"rgb(75,192,192)\"/>\n";
    svg << "<text x=\"585\" y=\"168\" font-size=\"14\" fill=\"#333\" font-weight=\"bold\">Listed Apps</text>\n";
    svg << "<text x=\"585\" y=\"185\" font-size=\"12\" fill=\"#666\">" 
        << formatTime(listedTime) << " (" << std::fixed << std::setprecision(1) 
        << listedPercent << "%)</text>\n";
    
    svg << "<rect x=\"550\" y=\"210\" width=\"25\" height=\"25\" fill=\"rgb(255,99,132)\"/>\n";
    svg << "<text x=\"585\" y=\"228\" font-size=\"14\" fill=\"#333\" font-weight=\"bold\">Unlisted Apps</text>\n";
    svg << "<text x=\"585\" y=\"245\" font-size=\"12\" fill=\"#666\">" 
        << formatTime(unlistedTime) << " (" << std::fixed << std::setprecision(1) 
        << unlistedPercent << "%)</text>\n";
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}

ChartOutput GraphicGenerator::createStatsBox(int distractionCount, long longestDistraction, 
                                             long longestFocus) {
    qDebug() << "[GraphicGenerator] Stats" << distractionCount << longestDistraction << longestFocus;

    ChartOutput output;
    output.title = "Attention Statistics";
    output.chartType = "stats";
    
    std::stringstream svg;
    svg << "<svg width=\"900\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Title
    svg << "<text x=\"450\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#0288D1\">Performance Metrics</text>\n";
    
    // 3 boxes with better spacing
    int boxWidth = 200;
    int boxHeight = 120;
    int spacing = 40;
    int startX = 50;
    int startY = 70;
    
    // Box 1: Distraction Count (Yellow)
    svg << "<rect x=\"" << startX << "\" y=\"" << startY << "\" width=\"" << boxWidth 
        << "\" height=\"" << boxHeight << "\" fill=\"#FFF8E1\" "
        << "stroke=\"#FBC02D\" stroke-width=\"3\" rx=\"12\"/>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 35) 
        << "\" text-anchor=\"middle\" font-size=\"13\" font-weight=\"bold\" fill=\"#F57F17\">Distractions</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 70) 
        << "\" text-anchor=\"middle\" font-size=\"36\" font-weight=\"bold\" "
        << "fill=\"#FBC02D\">" << distractionCount << "</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 100) 
        << "\" text-anchor=\"middle\" font-size=\"12\" fill=\"#F57F17\">times</text>\n";
    
    // Box 2: Longest Distraction (Red)
    startX += boxWidth + spacing;
    svg << "<rect x=\"" << startX << "\" y=\"" << startY << "\" width=\"" << boxWidth 
        << "\" height=\"" << boxHeight << "\" fill=\"#FFEBEE\" "
        << "stroke=\"#E53935\" stroke-width=\"3\" rx=\"12\"/>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 35) 
        << "\" text-anchor=\"middle\" font-size=\"13\" font-weight=\"bold\" fill=\"#C62828\">Longest Loss</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 70) 
        << "\" text-anchor=\"middle\" font-size=\"28\" font-weight=\"bold\" "
        << "fill=\"#E53935\">" << formatSeconds(longestDistraction) << "</text>\n";
    
    // Box 3: Longest Focus (Green)
    startX += boxWidth + spacing;
    svg << "<rect x=\"" << startX << "\" y=\"" << startY << "\" width=\"" << boxWidth 
        << "\" height=\"" << boxHeight << "\" fill=\"#E8F5E9\" "
        << "stroke=\"#43A047\" stroke-width=\"3\" rx=\"12\"/>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 35) 
        << "\" text-anchor=\"middle\" font-size=\"13\" font-weight=\"bold\" fill=\"#1B5E20\">Longest Focus</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 70) 
        << "\" text-anchor=\"middle\" font-size=\"28\" font-weight=\"bold\" "
        << "fill=\"#43A047\">" << formatSeconds(longestFocus) << "</text>\n";
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}
