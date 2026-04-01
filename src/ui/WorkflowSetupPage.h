#ifndef WORKFLOWSETUPPAGE_H
#define WORKFLOWSETUPPAGE_H

#include "Workflow.h"
#include "CustomSlider.h"
#include <QWidget>
#include <QStackedWidget>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QString>
#include <QMap>

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
    // UI Components
    QStackedWidget *stacked;
    
    // App Setup Page (Page 0)
    QLineEdit *appSearchInput;
    QListWidget *suggestionsListWidget;
    QListWidget *selectedAppsListWidget;
    
    // Duration Setup Page (Page 1)
    QLineEdit *workflowNameInput;
    CustomSlider *durationSlider;
    QLabel *timeLabel;
    QLabel *selectedEmojiLabel;
    
    // Data
    QString selectedEmoji;
    QMap<QString, QString> appIconMap;
    QStringList selectedApps;
    
    // UI Setup Methods
    void setupAppSetupPage();
    void setupDurationSetupPage();
    
    // Logic Methods
    void loadLinuxApps();
    void filterApps(const QString &searchText);
    void onAppAddClicked(const QString &appName);
    void onAppRemoveClicked(const QString &appName);
    void updateSelectedAppsList();
    void updateDurationLabel(int totalMinutes);
    void createWorkflow(int totalMinutes, bool isFavorite);
    void onBackClicked();
    void onStartWorkflowClicked();
    void onEmojiButtonClicked();
    void updateEmojiDisplay();
};

#endif // WORKFLOWSETUPPAGE_H
