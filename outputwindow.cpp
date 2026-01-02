#include "outputwindow.h"

#include "workflow.h"
#include "efficiency_meter.hpp"
#include "GraphicGenerator_header_and_structs.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <limits>


OutputWindow::OutputWindow(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);


    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);

    QLabel *title = new QLabel("FOCUS PATH", this);
    title->setStyleSheet("font-size:20px; font-weight:600;");
    topLayout->addSpacing(10);
    topLayout->addWidget(title);

    nameWorkflow = new QLabel("-", this);
    topLayout->addWidget(nameWorkflow);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background:#F8F9FA;");



    stacked = new QStackedWidget(this);

    workflowPage = new WorkflowPage();
    resultPage = new ResultPage();

    stacked->addWidget(workflowPage); // index 0
    stacked->addWidget(resultPage);   // index 1

    connect(workflowPage, &WorkflowPage::workflowCompleted,
            this, &OutputWindow::handleWorkflowCompleted);

    connect(resultPage, &ResultPage::backRequested, [=]() {
        stacked->setCurrentWidget(workflowPage);
        emit backToDashboardRequested();
    });
    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(line);
    mainLayout->addWidget(stacked,1);

    logsDir = QCoreApplication::applicationDirPath() + "/logs";

}
void OutputWindow::onWorkflowReady(Workflow &workflow)
{
    ownedWorkflow.reset();
    this->workflow = &workflow;
    workflowPage->getWorkflow(workflow);
    nameWorkflow->setText(
        QString::fromStdString(workflow.getName())
        );

    qDebug() << workflow.getName().c_str();
    qInfo() << workflow.getDuration();
    qInfo() << "onWorkflowReady called";

}

void OutputWindow::handleWorkflowCompleted()
{
    qDebug() << "[OutputWindow] Workflow completed signal received";

    loadResultsForCurrentWorkflow();
    stacked->setCurrentWidget(resultPage);
    emit workflowComplated();
}

QString OutputWindow::findLatestLogFile() const
{
    QDir dir(logsDir);
    dir.setNameFilters({"window_log_*.jsonl"});
    dir.setSorting(QDir::Time);
    const auto files = dir.entryInfoList(QDir::Files, QDir::Time);
    if (files.isEmpty()) {
        qWarning() << "[OutputWindow] No log files found in" << logsDir;
        return {};
    }
    return files.constFirst().absoluteFilePath();
}

QString OutputWindow::findLogForWorkflow(const Workflow &wf) const
{
    const QString stamp = QString::fromStdString(wf.getDate());
    const QDateTime target = QDateTime::fromString(stamp, "yyyy-MM-dd HH:mm:ss");
    if (!target.isValid()) {
        qWarning() << "[OutputWindow] Workflow date invalid; using latest log";
        return findLatestLogFile();
    }

    QDir dir(logsDir);
    dir.setNameFilters({"window_log_*.jsonl"});
    const auto files = dir.entryInfoList(QDir::Files, QDir::Time);
    if (files.isEmpty()) {
        qWarning() << "[OutputWindow] No log files found in" << logsDir;
        return {};
    }

    QString bestFile;
    qint64 bestDiff = std::numeric_limits<qint64>::max();

    for (const QFileInfo &fi : files) {
        const QString base = fi.baseName(); // window_log_YYYY-MM-DD_HH-MM
        const QString tsPart = base.mid(QString("window_log_").size());
        const QDateTime dt = QDateTime::fromString(tsPart, "yyyy-MM-dd_HH-mm");
        if (!dt.isValid()) continue;
        const qint64 diff = std::llabs(dt.secsTo(target));
        if (diff < bestDiff) {
            bestDiff = diff;
            bestFile = fi.absoluteFilePath();
        }
    }

    // If nothing matched, fall back to latest
    if (bestFile.isEmpty()) {
        return findLatestLogFile();
    }

    // Optional: require closeness (within 2 hours) to avoid wrong matches
    if (bestDiff > 7200) {
        qWarning() << "[OutputWindow] Closest log is" << bestDiff << "seconds away; using latest instead";
        return findLatestLogFile();
    }

    return bestFile;
}

void OutputWindow::loadResultsForCurrentWorkflow()
{
    if (!workflow) {
        qWarning() << "[OutputWindow] No workflow set; cannot load results";
        resultPage->showMessage("Workflow data missing; results unavailable.");
        return;
    }

    const QString logFile = findLogForWorkflow(*workflow);
    if (logFile.isEmpty()) {
        resultPage->showMessage("No log file found. Start tracking to generate results.");
        return;
    }

    qDebug() << "[OutputWindow] Loading results from" << logFile;

    try {
        EfficiencyMeter meter(logFile.toStdString(), workflow->getApps());
        meter.analyze();

        SessionData session = buildSessionData(meter);
        GraphicGenerator generator;
        SessionCharts charts = generator.generateCharts(session);

        saveChartsToDisk(charts, QString::fromStdString(workflow->getDate()));

        resultPage->showResults(
            meter.appUsage(),
            meter.attention(),
            logFile,
            workflow->getApps(),
            charts
        );
    } catch (const std::exception &ex) {
        qWarning() << "[OutputWindow] Failed to analyze log:" << ex.what();
        resultPage->showMessage(QString("Analysis failed: %1").arg(ex.what()));
    }
}

SessionData OutputWindow::buildSessionData(const EfficiencyMeter &meter) const
{
    SessionData data;

    // Totals by app
    for (const auto &pair : meter.totalsByApp()) {
        data.totalUsage[pair.first] = static_cast<long>(pair.second);
    }

    // Events timeline
    data.events.reserve(meter.events().size());
    for (const auto &ev : meter.events()) {
        AppEvent e;
        e.appName = ev.window_class;
        e.duration = static_cast<long>(ev.duration_ms);
        data.events.push_back(e);
    }

    // Usage pie data
    const auto usage = meter.appUsage();
    data.listedAppsTime = usage.listedTime;
    data.unlistedAppsTime = usage.unlistedTime;
    data.listedAppsPercent = usage.listedPercent;
    data.unlistedAppsPercent = usage.unlistedPercent;

    // Attention stats
    const auto att = meter.attention();
    data.distractionCount = att.distraction_count;
    data.longestDistraction = att.longest_distraction_ms / 1000; // convert to seconds
    data.longestFocus = att.longest_focus__ms / 1000; // convert to seconds

    return data;
}

void OutputWindow::saveChartsToDisk(const SessionCharts &charts, const QString &wfDate)
{
    const QString resultsDir = QCoreApplication::applicationDirPath() + "/results";
    QDir().mkpath(resultsDir);

    QString safeDate = wfDate;
    safeDate.replace(" ", "_");
    safeDate.replace(":", "-");

    auto saveOne = [&](const QString &name, const std::string &svg) {
        QFile f(resultsDir + "/" + safeDate + "_" + name + ".svg");
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QByteArray::fromStdString(svg));
            f.close();
        }
    };

    saveOne("usage", charts.usageTable.svgContent);
    saveOne("timeline", charts.timeline.svgContent);
    saveOne("pie", charts.pieChart.svgContent);
    saveOne("attention", charts.statsBox.svgContent);
}

void OutputWindow::showReportForWorkflow(const Workflow &wf)
{
    ownedWorkflow = std::make_unique<Workflow>(wf);
    workflow = ownedWorkflow.get();
    nameWorkflow->setText(QString::fromStdString(workflow->getName()));

    qDebug() << "[OutputWindow] Showing report for workflow" << QString::fromStdString(workflow->getName());
    loadResultsForCurrentWorkflow();
    stacked->setCurrentWidget(resultPage);
}


