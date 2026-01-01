#ifndef WORKFLOWSETUPPAGE_H
#define WORKFLOWSETUPPAGE_H

#include "clickablelabel.h"
#include "workflow.h"
#include <QWidget>
#include <QStackedWidget>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <string>

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QEvent;

class WorkflowSetupPage : public QWidget
{
    Q_OBJECT

public:
    explicit WorkflowSetupPage(QWidget *parent = nullptr);
    void loadTemplateWorkflow(const Workflow &workflow);

signals:
    void startWorkflow(Workflow &workflow);
    void backToDashboard();

private:

    QLineEdit   *templateName;
    QListWidget *allAppsList;
    QListWidget *selectedList;
    QStackedWidget *stacked;
    ClickableLabel *appsSetup;
    ClickableLabel *durationSetup;
    QLabel *timeLabel;
    QSlider *slider;
    QFrame *iconDisplayFrame;
    QLabel *iconLabel;
    QPushButton *startWorkflowButton;
    QPushButton *backButton;

    std::string selectedIcon;

    void loadLinuxApps();
    void onBackClicked();
    void onStartWorkflowClicked();
    void filterPopup(const QString &text);
    void showPopup();
    void closePopup();
    void updateLabel(int totalMinutes);
    void createWorkflow(int totalMinutes, bool isFavorite);
    void setupIconDisplay();
    void updateIconDisplay();
};

#endif // WORKFLOWSETUPPAGE_H
