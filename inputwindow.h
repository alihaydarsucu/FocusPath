#pragma once
#include <QWidget>
#include <QString>
#include "workflow.h"
#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>

// Forward declarations
class DashboardPage;
class WorkflowSetupPage;
class HistoryPage;
class ClickableLabel;


class InputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit InputWindow(QWidget *parent = nullptr);

public slots:
    void showDashboard();


signals:
    void requestOutputPage(Workflow &workflow);
    void startRequested();





private:
    QLineEdit *edit;
    ClickableLabel *dashboard;
    ClickableLabel *workflowSetup;
    ClickableLabel *history;
    ClickableLabel *templates;

    ClickableLabel *current = nullptr;

    QStackedWidget *stack;


};
