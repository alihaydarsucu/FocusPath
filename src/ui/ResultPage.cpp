#include "ResultPage.h"

#include <QFormLayout>
#include "ResultPage.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStringList>
#include <QSvgRenderer>
#include <QVBoxLayout>
#include <QPixmap>

ResultPage::ResultPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ResultPage::setupUI()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Modern gradient background
    this->setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FAFBFF, stop:0.5 #F0F6FF, stop:1 #E8F4FF); "
        "}"
    );

    // Header widget with logo and title
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #FFFFFF, stop:1 #F8FBFD); "
        "}"
    );
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 10, 10, 10);
    headerLayout->setSpacing(15);

    // Logo
    QLabel *logoLabel = new QLabel(this);
    QPixmap logo(":/pictures/FocusPath.png");
    logoLabel->setPixmap(logo.scaledToHeight(40, Qt::SmoothTransformation));
    headerLayout->addWidget(logoLabel);

    // Workflow name - centered
    workflowNameLabel = new QLabel("-", this);
    workflowNameLabel->setAlignment(Qt::AlignCenter);
    workflowNameLabel->setStyleSheet("font-size:18px; font-weight:600; color:#0288D1;");
    headerLayout->addWidget(workflowNameLabel, 1);

    // Status label
    statusLabel = new QLabel("Awaiting workflow results", this);
    statusLabel->setStyleSheet("color:#555; font-size: 12px;");
    headerLayout->addWidget(statusLabel);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(2);
    line->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #E0F7FF, stop:0.5 #0288D1, stop:1 #E0F7FF);");

    rootLayout->addWidget(headerWidget);
    rootLayout->addWidget(line);

    // Scroll area for content
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #F5F8FB, stop:1 #EDF3F8);"
        "  width: 12px;"
        "  border-radius: 6px;"
        "  margin: 2px 2px 2px 2px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0288D1, stop:1 #0AA4E8);"
        "  min-height: 30px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0277BD, stop:1 #0396D6);"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "  background: none;"
        "}"
        "QScrollBar:horizontal {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #F5F8FB, stop:1 #EDF3F8);"
        "  height: 12px;"
        "  border-radius: 6px;"
        "  margin: 2px 2px 2px 2px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0288D1, stop:1 #0AA4E8);"
        "  min-width: 30px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0277BD, stop:1 #0396D6);"
        "}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "  width: 0px;"
        "}"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
        "  background: none;"
        "}"
    );
    auto *scrollWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(scrollWidget);
    layout->setSpacing(16);
    layout->setContentsMargins(40, 20, 40, 20);
    
    // Set minimum width to prevent horizontal scrolling
    scrollWidget->setMinimumWidth(1000);

    auto *grid = new QGridLayout();
    grid->setVerticalSpacing(6);
    grid->setHorizontalSpacing(12);

    focusLabel = new QLabel("-", this);
    unfocusLabel = new QLabel("-", this);
    focusPercentLabel = new QLabel("-", this);
    distractPercentLabel = new QLabel("-", this);
    distractionCountLabel = new QLabel("-", this);
    longestFocusLabel = new QLabel("-", this);
    longestDistractLabel = new QLabel("-", this);
    appsLabel = new QLabel("-", this);
    logPathLabel = new QLabel("-", this);
    logPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    grid->addWidget(new QLabel("Focused time:", this), 0, 0);
    grid->addWidget(focusLabel, 0, 1);
    grid->addWidget(new QLabel("Unfocused time:", this), 1, 0);
    grid->addWidget(unfocusLabel, 1, 1);
    grid->addWidget(new QLabel("Focus %:", this), 2, 0);
    grid->addWidget(focusPercentLabel, 2, 1);
    grid->addWidget(new QLabel("Distraction %:", this), 3, 0);
    grid->addWidget(distractPercentLabel, 3, 1);
    grid->addWidget(new QLabel("Distraction count:", this), 4, 0);
    grid->addWidget(distractionCountLabel, 4, 1);
    grid->addWidget(new QLabel("Longest focus:", this), 5, 0);
    grid->addWidget(longestFocusLabel, 5, 1);
    grid->addWidget(new QLabel("Longest distraction:", this), 6, 0);
    grid->addWidget(longestDistractLabel, 6, 1);
    grid->addWidget(new QLabel("Tracked apps:", this), 7, 0);
    grid->addWidget(appsLabel, 7, 1);
    grid->addWidget(new QLabel("Log file:", this), 8, 0);
    grid->addWidget(logPathLabel, 8, 1);

    layout->addLayout(grid);

    // Create charts in proper side-by-side layout
    auto *chartsContainer = new QWidget();
    auto *chartsLayout = new QVBoxLayout(chartsContainer);
    chartsLayout->setSpacing(16);

    // Row 1: Usage Table and Timeline
    auto *row1 = new QHBoxLayout();
    auto addChartBoxForRow = [](QHBoxLayout *rowLayout, const QString &titleText, QSvgWidget *&widget) {
        auto *box = new QGroupBox(titleText);
        box->setStyleSheet(
            "QGroupBox { "
            "  border: 2px solid #E8F0F7; "
            "  border-radius: 8px; "
            "  padding: 10px; "
            "  color: #0288D1; "
            "  font-weight: bold; "
            "} "
            "QGroupBox::title { "
            "  subcontrol-origin: margin; "
            "  left: 10px; "
            "  padding: 0 3px; "
            "}"
        );
        auto *v = new QVBoxLayout(box);
        widget = new QSvgWidget(box);
        widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        v->addWidget(widget, 0, Qt::AlignHCenter);
        rowLayout->addWidget(box, 1);
    };

    addChartBoxForRow(row1, "Usage Table", usageChart);
    addChartBoxForRow(row1, "Timeline", timelineChart);
    chartsLayout->addLayout(row1);

    // Row 2: App Distribution and Attention Stats
    auto *row2 = new QHBoxLayout();
    addChartBoxForRow(row2, "App Distribution", pieChart);
    addChartBoxForRow(row2, "Attention Stats", statsChart);
    chartsLayout->addLayout(row2);

    layout->addWidget(chartsContainer);
    layout->addStretch();

    scrollArea->setWidget(scrollWidget);
    rootLayout->addWidget(scrollArea, 1);

    QPushButton *backBtn = new QPushButton("Back to Dashboard", this);
    backBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0288D1, stop:1 #0AA4E8);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 12px 30px;"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "  margin: 10px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0277BD, stop:1 #0396D6);"
        "}"
        "QPushButton:pressed {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #01579B, stop:1 #0277BD);"
        "}"
    );
    connect(backBtn, &QPushButton::clicked, this, &ResultPage::backRequested);
    rootLayout->addWidget(backBtn, 0, Qt::AlignRight);
}

void ResultPage::showResults(const AppUsageAnalyzer::AppUsageResult &usage,
                             const AttentionAnalyzer::AttentionStats &attention,
                             const QString &logFile,
                             const std::vector<std::string> &trackedApps,
                             const SessionCharts &charts)
{
    statusLabel->setText("Analysis complete");

    focusLabel->setText(formatMs(usage.listedTime));
    unfocusLabel->setText(formatMs(usage.unlistedTime));
    focusPercentLabel->setText(QString::number(usage.listedPercent, 'f', 1) + "%");
    distractPercentLabel->setText(QString::number(usage.unlistedPercent, 'f', 1) + "%");
    distractionCountLabel->setText(QString::number(attention.distractionCount));
    longestFocusLabel->setText(formatMs(attention.longestFocusMs));
    longestDistractLabel->setText(formatMs(attention.longestDistractionMs));

    QStringList apps;
    for (const auto &a : trackedApps) {
        apps << QString::fromStdString(a);
    }
    appsLabel->setText(apps.isEmpty() ? "No tracked apps (all apps counted)" : apps.join(", "));
    logPathLabel->setText(logFile);

    auto loadSvg = [](QSvgWidget *w, const std::string &svg) {
        if (!w) return;
        QByteArray arr(svg.c_str(), static_cast<int>(svg.size()));
        w->load(arr);
        if (auto renderer = w->renderer()) {
            renderer->setAspectRatioMode(Qt::KeepAspectRatio);
            QSize base = renderer->defaultSize();
            if (!base.isValid() || base.isEmpty()) {
                base = QSize(900, 360); // fall back to a sensible chart size
            }
            w->setMinimumSize(base);
            w->setMaximumSize(base);
            w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        }
    };

    loadSvg(usageChart, charts.usageTable.svgContent);
    loadSvg(timelineChart, charts.timeline.svgContent);
    loadSvg(pieChart, charts.pieChart.svgContent);
    loadSvg(statsChart, charts.statsBox.svgContent);
}

void ResultPage::showMessage(const QString &message)
{
    statusLabel->setText(message);
}

QString ResultPage::formatMs(long long ms)
{
    if (ms <= 0) return "0s";
    long long seconds = ms / 1000;
    long long minutes = seconds / 60;
    long long hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;

    if (hours > 0) {
        return QString("%1h %2m %3s").arg(hours).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    }
    if (minutes > 0) {
        return QString("%1m %2s").arg(minutes).arg(seconds, 2, 10, QChar('0'));
    }
    return QString::number(seconds) + "s";
}
void ResultPage::setWorkflowName(const QString &name)
{
    workflowNameLabel->setText(name);
}