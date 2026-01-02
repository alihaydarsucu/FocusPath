
#pragma once
#include <QWidget>
#include "workflow.h"
#include "workflowpage.h"
#include "resultpage.h"
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QString>

#include "GraphicGenerator_header_and_structs.h"
#include "efficiency_meter.hpp"

class OutputWindow : public QWidget {
    Q_OBJECT
public:
    explicit OutputWindow(QWidget *parent = nullptr);
private:
    Workflow *workflow = nullptr;
    std::unique_ptr<Workflow> ownedWorkflow;
    bool hasWorkflow = false;
    QLabel *nameWorkflow = nullptr;
    QListWidget *appsWorkflow = nullptr;
    WorkflowPage *workflowPage{};
    ResultPage *resultPage{};
    QStackedWidget *stacked{};
    QString logsDir;

    QString findLatestLogFile() const;
    QString findLogForWorkflow(const Workflow &wf) const;
    void loadResultsForCurrentWorkflow();
    SessionData buildSessionData(const EfficiencyMeter &meter) const;
    void saveChartsToDisk(const SessionCharts &charts, const QString &wfDate);
public slots:
    void onWorkflowReady(Workflow &workflow);
    void handleWorkflowCompleted();
    void showReportForWorkflow(const Workflow &workflow);
signals:
    void workflowComplated();
    void backToDashboardRequested();

};
