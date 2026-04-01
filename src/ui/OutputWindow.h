
#pragma once
#include <QWidget>
#include "Workflow.h"
#include "WorkflowPage.h"
#include "ResultPage.h"
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QString>

#include "GraphicGeneratorStructs.h"
#include "EfficiencyMeter.hpp"

class OutputWindow : public QWidget {
    Q_OBJECT
public:
    explicit OutputWindow(QWidget *parent = nullptr);
private:
    Workflow *workflow = nullptr;
    std::unique_ptr<Workflow> ownedWorkflow;
    bool hasWorkflow = false;
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
