#include "historypage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDate>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QtSvgWidgets/QSvgWidget>
#include <QRandomGenerator>
#include <algorithm>

HistoryPage::HistoryPage(QWidget *parent)
    : QWidget(parent), currentDateFilter(0), totalFocusTimeMs(0), averageEfficiency(0)
{
    setupUI();
    loadWorkflowHistory();
    calculateWeeklyStats();
    displayWeeklyOverview();
    
    // Defer session list population to ensure widget is fully initialized
    QTimer::singleShot(0, this, &HistoryPage::filterAndDisplaySessions);
}

void HistoryPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 32, 40, 40);
    mainLayout->setSpacing(32);
    
    this->setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FAFBFF, stop:0.5 #F0F6FF, stop:1 #E8F4FF); "
        "}"
    );

    // Header section
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *title = new QLabel("History");
    title->setStyleSheet(
        "font-size: 32px; "
        "font-weight: 700; "
        "color: #1F2937;"
    );
    
    QLabel *subtitle = new QLabel("Review your productivity trends and past session reports.");
    subtitle->setStyleSheet(
        "font-size: 14px; "
        "color: #6B7280;"
    );
    
    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);
    
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    
    mainLayout->addLayout(headerLayout);

    // Weekly Overview Section (smaller)
    QFrame *weeklyFrame = new QFrame();
    weeklyFrame->setObjectName("WeeklyFrame");
    weeklyFrame->setMaximumHeight(350);
    weeklyFrame->setMinimumHeight(350);
    weeklyFrame->setStyleSheet(
        "#WeeklyFrame { "
        "  background-color: #FFFFFF; "
        "  border-radius: 16px; "
        "  border: none; "
        "}"
    );
    
    QGraphicsDropShadowEffect *shadowWeekly = new QGraphicsDropShadowEffect(this);
    shadowWeekly->setBlurRadius(20);
    shadowWeekly->setXOffset(0);
    shadowWeekly->setYOffset(4);
    shadowWeekly->setColor(QColor(0, 0, 0, 8));
    
    QVBoxLayout *weeklyLayout = new QVBoxLayout(weeklyFrame);
    weeklyLayout->setContentsMargins(32, 28, 32, 28);
    weeklyLayout->setSpacing(16);
    
    // Header with title, stats, and filter on same line
    QHBoxLayout *weeklyHeaderLayout = new QHBoxLayout();
    
    QLabel *weeklyTitle = new QLabel("Weekly Overview");
    weeklyTitle->setStyleSheet(
        "font-size: 20px; "
        "font-weight: 700; "
        "color: #1F2937;"
    );
    weeklyHeaderLayout->addWidget(weeklyTitle);
    weeklyHeaderLayout->addSpacing(40);
    
    // Total Focus Time on same line
    totalFocusTimeLabel = new QLabel("0h 0m");
    totalFocusTimeLabel->setStyleSheet(
        "font-size: 28px; "
        "font-weight: 700; "
        "color: #3B9EDF;"
    );
    weeklyHeaderLayout->addWidget(totalFocusTimeLabel);
    
    weeklyHeaderLayout->addStretch();
    
    QLabel *filterLabelWeekly = new QLabel("Last 7 Days");
    filterLabelWeekly->setStyleSheet("font-size: 13px; color: #6B7280; font-weight: 500;");
    weeklyHeaderLayout->addWidget(filterLabelWeekly);
    
    weeklyLayout->addLayout(weeklyHeaderLayout);
    
    // Weekly chart placeholder
    weeklyChartWidget = new QSvgWidget();
    weeklyChartWidget->setMinimumSize(640, 260);
    weeklyChartWidget->setMaximumSize(640, 260);
    weeklyChartWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    weeklyLayout->addWidget(weeklyChartWidget, 0, Qt::AlignHCenter);
    
    mainLayout->addWidget(weeklyFrame);

    // Search and Filter Section
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(16);
    
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("🔍 Search by workflow name...");
    searchBox->setFixedHeight(42);
    searchBox->setFixedWidth(380);
    searchBox->setStyleSheet(
        "QLineEdit { "
        "  background-color: #FFFFFF; "
        "  border: 1px solid #E5E7EB; "
        "  border-radius: 10px; "
        "  padding: 0 16px; "
        "  font-size: 14px; "
        "  color: #1F2937; "
        "} "
        "QLineEdit:focus { "
        "  border: 1.5px solid #3B9EDF; "
        "  background-color: #FFFFFF; "
        "  outline: none; "
        "}"
    );
    
    connect(searchBox, &QLineEdit::textChanged, this, &HistoryPage::onSearchTextChanged);
    
    QLabel *filterLabel = new QLabel("Date Filter:");
    filterLabel->setStyleSheet("font-size: 13px; color: #6B7280; font-weight: 600;");
    
    dateFilterCombo = new QComboBox();
    dateFilterCombo->addItem("Last 7 Days");
    dateFilterCombo->addItem("Last 30 Days");
    dateFilterCombo->addItem("All Time");
    dateFilterCombo->setFixedHeight(42);
    dateFilterCombo->setFixedWidth(180);
    dateFilterCombo->setStyleSheet(
        "QComboBox { "
        "  background-color: #FFFFFF; "
        "  border: 1px solid #E5E7EB; "
        "  border-radius: 10px; "
        "  padding: 0 16px; "
        "  font-size: 14px; "
        "  color: #1F2937; "
        "} "
        "QComboBox:hover { "
        "  border: 1px solid #3B9EDF; "
        "} "
        "QComboBox:focus { "
        "  border: 1.5px solid #3B9EDF; "
        "  outline: none; "
        "} "
        "QComboBox::drop-down { "
        "  border: none; "
        "  width: 20px; "
        "} "
        "QComboBox QAbstractItemView { "
        "  background-color: #FFFFFF; "
        "  border: 1px solid #E5E7EB; "
        "  border-radius: 8px; "
        "  selection-background-color: #EFF6FF; "
        "  selection-color: #1F2937; "
        "}"
    );
    
    connect(dateFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &HistoryPage::onDateFilterChanged);
    
    controlsLayout->addWidget(searchBox);
    controlsLayout->addStretch();
    controlsLayout->addWidget(filterLabel);
    controlsLayout->addWidget(dateFilterCombo);
    
    mainLayout->addLayout(controlsLayout);

    // Sessions List Section
    QLabel *sessionsTitle = new QLabel("Recent Sessions");
    sessionsTitle->setStyleSheet(
        "font-size: 18px; "
        "font-weight: 700; "
        "color: #1F2937;"
    );
    mainLayout->addWidget(sessionsTitle);
    
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea { "
        "  background-color: #F9FAFB; "
        "  border: none; "
        "} "
        "QWidget#qt_scrollarea_viewport { "
        "  background-color: #F9FAFB; "
        "} "
        "QScrollBar:vertical { "
        "  background-color: #E5E7EB; "
        "  width: 10px; "
        "  border-radius: 5px; "
        "  margin: 0px; "
        "} "
        "QScrollBar::handle:vertical { "
        "  background-color: #3B9EDF; "
        "  border-radius: 5px; "
        "  min-height: 40px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "  background-color: #2B7EC4; "
        "}"
    );
    
    sessionsContainer = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(sessionsContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(14);
    
    sessionsLayout = new QGridLayout();
    sessionsLayout->setContentsMargins(0, 0, 0, 0);
    sessionsLayout->setSpacing(14);
    sessionsLayout->setColumnStretch(0, 1);
    
    containerLayout->addLayout(sessionsLayout);
    containerLayout->addStretch();
    
    scrollArea->setWidget(sessionsContainer);
    mainLayout->addWidget(scrollArea);
}

void HistoryPage::loadWorkflowHistory() {
    QString workflowsDir = QCoreApplication::applicationDirPath() + "/workflows";
    QDir dir(workflowsDir);
    
    if (!dir.exists()) {
        qWarning() << "Workflows directory not found:" << workflowsDir;
        return;
    }
    
    // Get all JSON files sorted by name (which is the timestamp)
    QStringList files = dir.entryList(QStringList() << "*.json", QDir::Files, QDir::Time | QDir::Reversed);
    
    allWorkflows.clear();
    workflowFileMap.clear();
    
    for (const QString &fileName : files) {
        QString filePath = dir.filePath(fileName);
        
        try {
            Workflow wf = Workflow::loadWorkflowFromFile(filePath);
            allWorkflows.append(wf);
            workflowFileMap[QString::fromStdString(wf.getDate())] = filePath;
        } catch (const std::exception &e) {
            qWarning() << "Failed to load workflow:" << fileName << "-" << e.what();
        }
    }
}

void HistoryPage::calculateWeeklyStats() {
    QDate today = QDate::currentDate();
    QDate weekStart = today.addDays(-(today.dayOfWeek() - 1));  // Monday of this week
    
    totalFocusTimeMs = 0;
    int totalEfficiency = 0;
    int count = 0;
    
    for (const Workflow &wf : allWorkflows) {
        QDate wfDate = QDate::fromString(QString::fromStdString(wf.getDate()), "yyyy-MM-dd HH:mm:ss");
        
        if (wfDate.isValid() && wfDate >= weekStart && wfDate <= today) {
            totalFocusTimeMs += wf.getDuration() * 60 * 1000;  // Convert minutes to ms
            count++;
        }
    }
    
    // Convert milliseconds to hours and minutes
    long long totalSeconds = totalFocusTimeMs / 1000;
    long long hours = totalSeconds / 3600;
    long long minutes = (totalSeconds % 3600) / 60;
    
    // Update label
    totalFocusTimeLabel->setText(QString("%1h %2m").arg(hours).arg(minutes));
}

void HistoryPage::displayWeeklyOverview() {
    QDate today = QDate::currentDate();
    QDate weekStart = today.addDays(-(today.dayOfWeek() - 1)); // Monday

    // Sum minutes per day for the current week
    QVector<long long> minutesByDay(7, 0);
    for (const Workflow &wf : allWorkflows) {
        const QString stamp = QString::fromStdString(wf.getDate());
        const QDate d = QDate::fromString(stamp, "yyyy-MM-dd HH:mm:ss");
        if (!d.isValid()) continue;
        const int offset = weekStart.daysTo(d);
        if (offset >= 0 && offset < 7) {
            minutesByDay[offset] += wf.getDuration();
        }
    }

    const long long maxMinutes = *std::max_element(minutesByDay.constBegin(), minutesByDay.constEnd());
    const int width = 640;
    const int height = 260;
    const int chartHeight = 200;
    const int barWidth = 60;
    const int gap = 25;
    const int baseY = 230;
    const QStringList days = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};

    QString svg;
    svg += QString("<svg width='%1' height='%2' xmlns='http://www.w3.org/2000/svg'>").arg(width).arg(height);
    svg += "<rect width='100%' height='100%' fill='#F9FAFB' rx='12'/>";

    int x = 26;
    for (int i = 0; i < minutesByDay.size(); ++i) {
        const long long minutes = minutesByDay[i];
        const double ratio = (maxMinutes > 0) ? static_cast<double>(minutes) / maxMinutes : 0.0;
        const int h = static_cast<int>(ratio * chartHeight);
        const int y = baseY - h;

        svg += QString("<rect x='%1' y='%2' width='%3' height='%4' rx='10' fill='#3B9EDF'/>")
                   .arg(x).arg(y).arg(barWidth).arg(h);
        svg += QString("<text x='%1' y='%2' text-anchor='middle' font-size='11' fill='#1F2937'>%3</text>")
                   .arg(x + barWidth / 2).arg(baseY + 18).arg(days[i]);
        svg += QString("<text x='%1' y='%2' text-anchor='middle' font-size='10' fill='#111827'>%3m</text>")
                   .arg(x + barWidth / 2).arg(y - 6).arg(minutes);
        x += barWidth + gap;
    }

    svg += "</svg>";
    if (weeklyChartWidget) {
        weeklyChartWidget->load(svg.toUtf8());
    }
}

void HistoryPage::displaySessionsList() {
    // Safety check
    if (!sessionsLayout) {
        qWarning() << "sessionsLayout is null!";
        return;
    }
    
    // Clear existing items
    while (sessionsLayout->count() > 0) {
        QLayoutItem *item = sessionsLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    if (filteredWorkflows.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No sessions found");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(
            "font-size: 14px; "
            "color: #6B7280; "
            "padding: 40px;"
        );
        sessionsLayout->addWidget(emptyLabel, 0, 0);
        return;
    }
    
    // Display each session as a card
    int row = 0;
    for (Workflow &wf : filteredWorkflows) {
        QFrame *sessionFrame = new QFrame();
        sessionFrame->setObjectName("SessionFrame");
        sessionFrame->setStyleSheet(
            "#SessionFrame { "
            "  background-color: #FFFFFF; "
            "  border: 1px solid #E5E7EB; "
            "  border-radius: 12px; "
            "} "
            "#SessionFrame:hover { "
            "  border: 1px solid #D1D5DB; "
            "  background-color: #FAFAFA; "
            "}"
        );
        sessionFrame->setCursor(Qt::PointingHandCursor);
        
        QHBoxLayout *sessionLayout = new QHBoxLayout(sessionFrame);
        sessionLayout->setContentsMargins(20, 16, 20, 16);
        sessionLayout->setSpacing(20);
        
        // Date and time - LEFT
        QVBoxLayout *dateTimeLayout = new QVBoxLayout();
        dateTimeLayout->setSpacing(4);
        QString dateStr = QString::fromStdString(wf.getDate());
        QDate wfDate = QDate::fromString(dateStr, "yyyy-MM-dd HH:mm:ss");
        QString displayDate = wfDate.toString("dd MMM").toUpper();
        QString displayTime = dateStr.mid(11, 5);  // Extract HH:MM
        
        QLabel *dateLabel = new QLabel(displayDate);
        dateLabel->setStyleSheet(
            "font-size: 13px; "
            "color: #1F2937; "
            "font-weight: 700;"
        );
        QLabel *timeLabel = new QLabel(displayTime);
        timeLabel->setStyleSheet(
            "font-size: 12px; "
            "color: #9CA3AF; "
            "font-weight: 500;"
        );
        dateTimeLayout->addWidget(dateLabel);
        dateTimeLayout->addWidget(timeLabel);
        dateTimeLayout->addStretch();
        
        sessionLayout->addLayout(dateTimeLayout);
        
        // Icon - CENTER-LEFT (use same logic as WorkflowCard)
        QFrame *iconCircle = new QFrame();
        iconCircle->setFixedSize(56, 56);
        iconCircle->setObjectName("SessionIcon");
        
        // Get workflow name
        QString workflowName = QString::fromStdString(wf.getName());
        QString lowerName = workflowName.toLower();
        
        // Use same color logic as WorkflowCard::getColorForWorkflow
        QColor bgColor;
        if (lowerName.contains("code") || lowerName.contains("program")) {
            bgColor = QColor(50, 50, 50);  // Dark for coding
        } else if (lowerName.contains("design")) {
            bgColor = QColor(255, 87, 34);  // Orange for design
        } else if (lowerName.contains("read")) {
            bgColor = QColor(255, 193, 7);  // Yellow for reading
        } else if (lowerName.contains("email") || lowerName.contains("mail")) {
            bgColor = QColor(66, 133, 244);  // Blue for email
        } else {
            // Default colors based on hash of name
            uint hash = qHash(workflowName);
            const QColor defaultColors[] = {
                QColor(102, 187, 106),  // Green
                QColor(66, 133, 244),   // Blue
                QColor(171, 71, 188),   // Purple
                QColor(255, 112, 67),   // Orange-Red
                QColor(38, 166, 154)    // Teal
            };
            bgColor = defaultColors[hash % 5];
        }
        
        iconCircle->setStyleSheet(
            QString("#SessionIcon { background-color: %1; border-radius: 14px; }")
            .arg(bgColor.name())
        );
        
        // Get icon from workflow (same as WorkflowCard)
        QString iconFromWorkflow = QString::fromStdString(wf.getIcon());
        QLabel *iconLabel = new QLabel();
        iconLabel->setAlignment(Qt::AlignCenter);
        
        if (!iconFromWorkflow.isEmpty()) {
            iconLabel->setText(iconFromWorkflow);
            iconLabel->setStyleSheet(
                "border: none; "
                "background: transparent; "
                "font-size: 24px; "
                "font-weight: bold; "
                "color: white;"
            );
        } else {
            // Default icon - show first letter
            iconLabel->setText(workflowName.left(1).toUpper());
            iconLabel->setStyleSheet(
                "border: none; "
                "background: transparent; "
                "font-size: 24px; "
                "font-weight: bold; "
                "color: white;"
            );
        }
        
        QVBoxLayout *iconLayout = new QVBoxLayout(iconCircle);
        iconLayout->setContentsMargins(0, 0, 0, 0);
        iconLayout->addWidget(iconLabel);
        
        sessionLayout->addWidget(iconCircle);
        
        // Content - CENTER-RIGHT
        QVBoxLayout *contentLayout = new QVBoxLayout();
        contentLayout->setSpacing(6);
        
        QLabel *nameLabel = new QLabel(QString::fromStdString(wf.getName()));
        nameLabel->setStyleSheet(
            "font-size: 15px; "
            "font-weight: 700; "
            "color: #1F2937;"
        );
        contentLayout->addWidget(nameLabel);
        
        // Apps list
        const auto &apps = wf.getApps();
        QString appsStr;
        if (apps.empty()) {
            appsStr = "No apps tracked";
        } else {
            size_t maxAppsToShow = std::min(size_t(3), apps.size());
            for (size_t i = 0; i < maxAppsToShow; ++i) {
                if (i < apps.size()) {
                    appsStr += QString::fromStdString(apps[i]);
                    if (i < maxAppsToShow - 1 && i < apps.size() - 1) {
                        appsStr += " • ";
                    }
                }
            }
            if (apps.size() > 3) {
                appsStr += QString(" +%1").arg(apps.size() - 3);
            }
        }
        
        QLabel *appsLabel = new QLabel(appsStr);
        appsLabel->setStyleSheet(
            "font-size: 12px; "
            "color: #9CA3AF; "
            "font-weight: 500;"
        );
        appsLabel->setWordWrap(true);
        contentLayout->addWidget(appsLabel);
        contentLayout->addStretch();
        
        sessionLayout->addLayout(contentLayout, 1);
        
        // Duration - RIGHT
        long long minutes = wf.getDuration();
        long long hours = minutes / 60;
        long long mins = minutes % 60;
        
        QLabel *durationLabel = new QLabel(
            hours > 0 
                ? QString("⏱ %1h %2m").arg(hours).arg(mins)
                : QString("⏱ %1m").arg(mins)
        );
        durationLabel->setAlignment(Qt::AlignRight);
        durationLabel->setStyleSheet(
            "font-size: 15px; "
            "font-weight: 700; "
            "color: #3B9EDF; "
            "min-width: 80px;"
        );
        sessionLayout->addWidget(durationLabel);
        
        // Efficiency placeholder
        QLabel *efficiencyLabel = new QLabel("—");
        efficiencyLabel->setAlignment(Qt::AlignCenter);
        efficiencyLabel->setFixedWidth(70);
        efficiencyLabel->setStyleSheet(
            "font-size: 12px; "
            "color: #9CA3AF; "
            "font-weight: 600;"
        );
        sessionLayout->addWidget(efficiencyLabel);
        
        // Start Session button
        QPushButton *startButton = new QPushButton("▶ Start");
        startButton->setFixedSize(95, 40);
        startButton->setStyleSheet(
            "QPushButton { "
            "  background-color: #E94B4B; "
            "  color: white; "
            "  border: none; "
            "  border-radius: 10px; "
            "  font-size: 13px; "
            "  font-weight: 600; "
            "  padding: 0px; "
            "} "
            "QPushButton:hover { "
            "  background-color: #D63838; "
            "} "
            "QPushButton:pressed { "
            "  background-color: #C42A2A; "
            "}"
        );
        startButton->setCursor(Qt::PointingHandCursor);
        connect(startButton, &QPushButton::clicked, this, [this, wf]() {
            // Keep a persistent copy so downstream users don't see a dangling reference
            activeWorkflowFromHistory = std::make_unique<Workflow>(wf);
            emit startWorkflow(*activeWorkflowFromHistory);
        });
        sessionLayout->addWidget(startButton);
        
        // View Report button
        QPushButton *viewButton = new QPushButton("View Report →");
        viewButton->setFixedSize(130, 40);
        viewButton->setStyleSheet(
            "QPushButton { "
            "  background-color: transparent; "
            "  color: #3B9EDF; "
            "  border: 1.5px solid #3B9EDF; "
            "  border-radius: 10px; "
            "  font-size: 12px; "
            "  font-weight: 600; "
            "  padding: 0px; "
            "} "
            "QPushButton:hover { "
            "  background-color: #3B9EDF; "
            "  color: white; "
            "} "
            "QPushButton:pressed { "
            "  background-color: #2B7EC4; "
            "  border-color: #2B7EC4; "
            "}"
        );
        viewButton->setCursor(Qt::PointingHandCursor);
        connect(viewButton, &QPushButton::clicked, this, [this, wf]() {
            emit viewReportRequested(wf);
        });
        sessionLayout->addWidget(viewButton);
        
        sessionsLayout->addWidget(sessionFrame, row, 0);
        row++;
    }
}

void HistoryPage::filterAndDisplaySessions() {
    filteredWorkflows.clear();
    
    QDate today = QDate::currentDate();
    QDate filterDate;
    
    // Set filter date based on combo box
    switch (currentDateFilter) {
        case 0:  // Last 7 days
            filterDate = today.addDays(-7);
            break;
        case 1:  // Last 30 days
            filterDate = today.addDays(-30);
            break;
        case 2:  // All time
            filterDate = QDate(1900, 1, 1);
            break;
        default:
            filterDate = today.addDays(-7);
    }
    
    // Filter workflows
    for (const Workflow &wf : allWorkflows) {
        QString dateStr = QString::fromStdString(wf.getDate());
        QDate wfDate = QDate::fromString(dateStr, "yyyy-MM-dd HH:mm:ss");
        
        // Check date filter
        if (wfDate.isValid() && wfDate < filterDate) {
            continue;
        }
        
        // Check search filter
        if (!currentSearchText.isEmpty()) {
            QString workflowName = QString::fromStdString(wf.getName());
            if (!workflowName.contains(currentSearchText, Qt::CaseInsensitive)) {
                continue;
            }
        }
        
        filteredWorkflows.append(wf);
    }
    
    displaySessionsList();
}

void HistoryPage::onSearchTextChanged(const QString &text) {
    currentSearchText = text;
    filterAndDisplaySessions();
}

void HistoryPage::onDateFilterChanged(int index) {
    currentDateFilter = index;
    calculateWeeklyStats();
    displayWeeklyOverview();
    filterAndDisplaySessions();
}

void HistoryPage::onSessionClicked(const Workflow &workflow) {
    // Could be extended to show detailed report
    qDebug() << "Session clicked:" << QString::fromStdString(workflow.getName());
}

void HistoryPage::refreshHistory() {
    loadWorkflowHistory();
    calculateWeeklyStats();
    displayWeeklyOverview();
    filterAndDisplaySessions();
}

long long HistoryPage::getTotalFocusTime() const {
    return totalFocusTimeMs;
}

int HistoryPage::calculateAverageEfficiency() const {
    return averageEfficiency;
}

double HistoryPage::getEfficiencyForWorkflow(const Workflow &workflow) const {
    // This would require analyzing log files for the specific workflow
    // For now, returning 0 as placeholder
    return 0.0;
}
