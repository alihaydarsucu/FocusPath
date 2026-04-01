#ifndef WORKFLOWPAGE_H
#define WORKFLOWPAGE_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

class Workflow;

class WorkflowPage : public QWidget
{
    Q_OBJECT

public:
    explicit WorkflowPage(QWidget *parent = nullptr);
    void getWorkflow(Workflow &workflow);
    void onBackClicked();
signals:

    void workflowCompleted();

private:
    void setupUI();
    void updateCountdown();
    Workflow *workflow = nullptr;
    QTimer *countdownTimer = nullptr;
    QLabel *countdownLabel = nullptr;
    int remainingSeconds = 0;
};

#endif // WORKFLOWPAGE_H
