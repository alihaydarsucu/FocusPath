
#pragma once
#include <QWidget>
#include "workflow.h"
#include "workflowpage.h"
#include "resultpage.h"
#include <QLabel>
#include <QListWidget>

class OutputWindow : public QWidget {
    Q_OBJECT
public:
    explicit OutputWindow(QWidget *parent = nullptr);
private:
    Workflow *workflow = nullptr;
    bool hasWorkflow = false;
    QLabel *nameWorkflow = nullptr;
    QListWidget *appsWorkflow = nullptr;
    WorkflowPage *workflowPage;
public slots:
    void onWorkflowReady(Workflow &workflow);
signals:
    void workflowComplated();

};
