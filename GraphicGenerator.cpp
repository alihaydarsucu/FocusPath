#include "GraphicGenerator_header_and_structs.h"
#include <sstream>
#include <iomanip>
#include <cmath>

GraphicGenerator::GraphicGenerator() : darkMode(false) {}

void GraphicGenerator::setDarkMode(bool enable) {
    darkMode = enable;
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
    ChartOutput output;
    output.title = "Uygulama Kullanım Tablosu";
    output.chartType = "table";
    
    std::stringstream svg;
    svg << "<svg width=\"600\" height=\"" << (80 + totalUsage.size() * 40) 
        << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Başlık
    svg << "<text x=\"300\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#333\">Toplam Kullanım Süreleri</text>\n";
    
    // Tablo başlıkları
    svg << "<rect x=\"50\" y=\"50\" width=\"500\" height=\"40\" fill=\"#667eea\" "
        << "stroke=\"#333\" stroke-width=\"2\"/>\n";
    svg << "<text x=\"150\" y=\"75\" text-anchor=\"middle\" font-size=\"16\" "
        << "font-weight=\"bold\" fill=\"white\">Uygulama</text>\n";
    svg << "<text x=\"450\" y=\"75\" text-anchor=\"middle\" font-size=\"16\" "
        << "font-weight=\"bold\" fill=\"white\">Süre</text>\n";
    
    // Satırlar
    int y = 90;
    int index = 0;
    for (const auto& pair : totalUsage) {
        std::string rowColor = (index % 2 == 0) ? "#f8f9fa" : "#ffffff";
        
        svg << "<rect x=\"50\" y=\"" << y << "\" width=\"500\" height=\"40\" "
            << "fill=\"" << rowColor << "\" stroke=\"#dee2e6\"/>\n";
        
        svg << "<text x=\"150\" y=\"" << (y + 25) << "\" text-anchor=\"middle\" "
            << "font-size=\"14\" fill=\"#333\">" << pair.first << "</text>\n";
        
        svg << "<text x=\"450\" y=\"" << (y + 25) << "\" text-anchor=\"middle\" "
            << "font-size=\"14\" font-weight=\"bold\" fill=\"#667eea\">" 
            << formatTime(pair.second) << "</text>\n";
        
        y += 40;
        index++;
    }
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}

ChartOutput GraphicGenerator::createTimeline(const std::vector<AppEvent>& events) {
    ChartOutput output;
    output.title = "Aktivite Zaman Çizelgesi";
    output.chartType = "timeline";
    
    // Toplam süreyi hesapla
    long totalTime = 0;
    for (const auto& event : events) {
        totalTime += event.duration;
    }
    
    std::stringstream svg;
    svg << "<svg width=\"900\" height=\"250\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Başlık
    svg << "<text x=\"450\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#333\">Zaman Çizelgesi</text>\n";
    
    // Timeline çizgisi
    double timelineWidth = 800;
    double currentX = 50;
    int colorIndex = 0;
    
    for (const auto& event : events) {
        double width = (double)event.duration / totalTime * timelineWidth;
        
        // Segment
        svg << "<rect x=\"" << currentX << "\" y=\"80\" width=\"" << width 
            << "\" height=\"60\" fill=\"" << getColor(colorIndex) 
            << "\" stroke=\"white\" stroke-width=\"2\"/>\n";
        
        // Etiket (eğer yeteri kadar genişse)
        if (width > 40) {
            svg << "<text x=\"" << (currentX + width/2) << "\" y=\"110\" "
                << "text-anchor=\"middle\" font-size=\"12\" font-weight=\"bold\" "
                << "fill=\"white\">" << event.appName << "</text>\n";
            
            svg << "<text x=\"" << (currentX + width/2) << "\" y=\"125\" "
                << "text-anchor=\"middle\" font-size=\"10\" fill=\"white\">" 
                << formatTime(event.duration) << "</text>\n";
        }
        
        currentX += width;
        colorIndex++;
    }
    
    // Zaman ekseni
    svg << "<line x1=\"50\" y1=\"160\" x2=\"850\" y2=\"160\" "
        << "stroke=\"#333\" stroke-width=\"2\"/>\n";
    
    svg << "<text x=\"50\" y=\"180\" font-size=\"12\" fill=\"#666\">0s</text>\n";
    svg << "<text x=\"850\" y=\"180\" text-anchor=\"end\" font-size=\"12\" "
        << "fill=\"#666\">" << formatTime(totalTime) << "</text>\n";
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}

ChartOutput GraphicGenerator::createPieChart(long listedTime, double listedPercent, 
                                             long unlistedTime, double unlistedPercent) {
    ChartOutput output;
    output.title = "Listelenen/Listelenmayan Uygulamalar";
    output.chartType = "pie";
    
    std::stringstream svg;
    svg << "<svg width=\"600\" height=\"500\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Başlık
    svg << "<text x=\"300\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#333\">Uygulama Dağılımı</text>\n";
    
    double cx = 200, cy = 250, r = 120;
    
    // Listed apps (yeşil)
    double listedAngle = listedPercent * 3.6;  // Yüzdeyi açıya çevir
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
    
    // Unlisted apps (kırmızı)
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
    
    // Legend
    svg << "<rect x=\"400\" y=\"150\" width=\"25\" height=\"25\" fill=\"rgb(75,192,192)\"/>\n";
    svg << "<text x=\"435\" y=\"168\" font-size=\"14\" fill=\"#333\">Listed Apps</text>\n";
    svg << "<text x=\"435\" y=\"185\" font-size=\"12\" fill=\"#666\">" 
        << formatTime(listedTime) << " (" << std::fixed << std::setprecision(1) 
        << listedPercent << "%)</text>\n";
    
    svg << "<rect x=\"400\" y=\"210\" width=\"25\" height=\"25\" fill=\"rgb(255,99,132)\"/>\n";
    svg << "<text x=\"435\" y=\"228\" font-size=\"14\" fill=\"#333\">Unlisted Apps</text>\n";
    svg << "<text x=\"435\" y=\"245\" font-size=\"12\" fill=\"#666\">" 
        << formatTime(unlistedTime) << " (" << std::fixed << std::setprecision(1) 
        << unlistedPercent << "%)</text>\n";
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}

ChartOutput GraphicGenerator::createStatsBox(int distractionCount, long longestDistraction, 
                                             long longestFocus) {
    ChartOutput output;
    output.title = "Dikkat Dağınıklığı İstatistikleri";
    output.chartType = "stats";
    
    std::stringstream svg;
    svg << "<svg width=\"600\" height=\"300\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Başlık
    svg << "<text x=\"300\" y=\"30\" text-anchor=\"middle\" font-size=\"20\" "
        << "font-weight=\"bold\" fill=\"#333\">Performans Metrikleri</text>\n";
    
    // 3 kutu
    int boxWidth = 160;
    int boxHeight = 120;
    int spacing = 30;
    int startX = 70;
    int startY = 70;
    
    // Kutu 1: Distraction Count
    svg << "<rect x=\"" << startX << "\" y=\"" << startY << "\" width=\"" << boxWidth 
        << "\" height=\"" << boxHeight << "\" fill=\"#fff3cd\" "
        << "stroke=\"#ffc107\" stroke-width=\"3\" rx=\"10\"/>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 35) 
        << "\" text-anchor=\"middle\" font-size=\"14\" fill=\"#856404\">Dikkat Dağılması</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 75) 
        << "\" text-anchor=\"middle\" font-size=\"36\" font-weight=\"bold\" "
        << "fill=\"#ffc107\">" << distractionCount << "</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 105) 
        << "\" text-anchor=\"middle\" font-size=\"12\" fill=\"#856404\">kez</text>\n";
    
    // Kutu 2: Longest Distraction
    startX += boxWidth + spacing;
    svg << "<rect x=\"" << startX << "\" y=\"" << startY << "\" width=\"" << boxWidth 
        << "\" height=\"" << boxHeight << "\" fill=\"#f8d7da\" "
        << "stroke=\"#dc3545\" stroke-width=\"3\" rx=\"10\"/>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 35) 
        << "\" text-anchor=\"middle\" font-size=\"14\" fill=\"#721c24\">En Uzun Dağılma</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 75) 
        << "\" text-anchor=\"middle\" font-size=\"28\" font-weight=\"bold\" "
        << "fill=\"#dc3545\">" << formatSeconds(longestDistraction) << "</text>\n";
    
    // Kutu 3: Longest Focus
    startX += boxWidth + spacing;
    svg << "<rect x=\"" << startX << "\" y=\"" << startY << "\" width=\"" << boxWidth 
        << "\" height=\"" << boxHeight << "\" fill=\"#d4edda\" "
        << "stroke=\"#28a745\" stroke-width=\"3\" rx=\"10\"/>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 35) 
        << "\" text-anchor=\"middle\" font-size=\"14\" fill=\"#155724\">En Uzun Odaklanma</text>\n";
    
    svg << "<text x=\"" << (startX + boxWidth/2) << "\" y=\"" << (startY + 75) 
        << "\" text-anchor=\"middle\" font-size=\"28\" font-weight=\"bold\" "
        << "fill=\"#28a745\">" << formatSeconds(longestFocus) << "</text>\n";
    
    svg << "</svg>";
    output.svgContent = svg.str();
    return output;
}
