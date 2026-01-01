#ifndef WORKFLOWSETUPPAGE_H
#define WORKFLOWSETUPPAGE_H

#include "clickablelabel.h"
#include "workflow.h"
#include <QWidget>
#include <QStackedWidget>
#include <QSlider>
#include <QLabel>

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QEvent;

class WorkflowSetupPage : public QWidget
{
    Q_OBJECT

public:
    explicit WorkflowSetupPage(QWidget *parent = nullptr);

protected:

signals:

    void startWorkflow(Workflow &workflow);

private:

    QLineEdit   *templateName;
    QListWidget *allAppsList;
    QListWidget *selectedList;
    QStackedWidget *stacked;
    ClickableLabel *appsSetup;
    ClickableLabel *durationSetup;
    QLabel *timeLabel;
    QSlider *slider;


    void loadLinuxApps();
    void onBackClicked();
    void filterPopup(const QString &text);
    void showPopup();
    void closePopup();
    void updateLabel(int totalMinutes);
    void createWorkflow(int totalMinutes, bool isFavorite);
};

#endif // WORKFLOWSETUPPAGE_H
