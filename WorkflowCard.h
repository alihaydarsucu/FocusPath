#ifndef WORKFLOWCARD_H
#define WORKFLOWCARD_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QStringList>
#include "workflow.h"

class WorkflowCard : public QFrame {
    Q_OBJECT

public:
    explicit WorkflowCard(const Workflow &workflow, QWidget *parent = nullptr);

    QString getWorkflowName() const { return workflowName; }
    const Workflow& getWorkflow() const { return workflow; }

signals:
    void startSessionClicked(const Workflow &workflow);
    void editClicked(const Workflow &workflow);
    void deleteClicked(const Workflow &workflow);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupUI();
    void setData();
    QString getIconPathForWorkflow(const QString &name);
    QColor getColorForWorkflow(const QString &name);

    Workflow workflow;
    QString workflowName;
    int duration;
    QStringList apps;

    QFrame *iconCircle;
    QLabel *iconLabel;
    QLabel *nameLabel;
    QLabel *durationLabel;
    QLabel *appsLabel;
    QPushButton *startButton;
    QPushButton *menuButton;
};

#endif // WORKFLOWCARD_H
