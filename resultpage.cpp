#include "resultpage.h"

#include <QFormLayout>
#include "resultpage.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStringList>
#include <QSvgRenderer>
#include <QVBoxLayout>

ResultPage::ResultPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ResultPage::setupUI()
{
    auto *rootLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Results", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold;");
    rootLayout->addWidget(title);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    auto *scrollWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(scrollWidget);
    layout->setSpacing(16);

    statusLabel = new QLabel("Awaiting workflow results", this);
    statusLabel->setStyleSheet("color:#555; font-size: 13px;");
    layout->addWidget(statusLabel);

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

    auto addChartBox = [layout](const QString &titleText, QSvgWidget *&widget) {
        auto *box = new QGroupBox(titleText);
        auto *v = new QVBoxLayout(box);
        widget = new QSvgWidget(box);
        widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        v->addWidget(widget, 0, Qt::AlignHCenter);
        layout->addWidget(box);
    };

    addChartBox("Usage Table", usageChart);
    addChartBox("Timeline", timelineChart);
    addChartBox("App Distribution", pieChart);
    addChartBox("Attention Stats", statsChart);

    layout->addStretch();

    scrollArea->setWidget(scrollWidget);
    rootLayout->addWidget(scrollArea, 1);

    QPushButton *backBtn = new QPushButton("Back to Dashboard", this);
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
    distractionCountLabel->setText(QString::number(attention.distraction_count));
    longestFocusLabel->setText(formatMs(attention.longest_focus__ms));
    longestDistractLabel->setText(formatMs(attention.longest_distraction_ms));

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
