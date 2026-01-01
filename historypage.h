#ifndef HISTORYPAGE_H
#define HISTORYPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QHash>
#include <QDate>
#include "workflow.h"

class HistoryPage : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryPage(QWidget *parent = nullptr);
    void refreshHistory();

signals:
    void startWorkflow(Workflow &workflow);

private slots:
    void onSearchTextChanged(const QString &text);
    void onDateFilterChanged(int index);
    void onSessionClicked(const Workflow &workflow);

private:
    void setupUI();
    void loadWorkflowHistory();
    void displayWeeklyOverview();
    void displaySessionsList();
    void filterAndDisplaySessions();
    void calculateWeeklyStats();
    
    // Helper functions
    long long getTotalFocusTime() const;
    int calculateAverageEfficiency() const;
    double getEfficiencyForWorkflow(const Workflow &workflow) const;

    // UI Components - Header
    QLineEdit *searchBox;
    QComboBox *dateFilterCombo;
    
    // UI Components - Weekly Overview
    QLabel *totalFocusTimeLabel;
    QLabel *weeklyChartLabel;  // Placeholder for chart
    
    // UI Components - Sessions List
    QScrollArea *scrollArea;
    QWidget *sessionsContainer;
    QGridLayout *sessionsLayout;
    
    // Data
    QVector<Workflow> allWorkflows;
    QVector<Workflow> filteredWorkflows;
    QHash<QString, QString> workflowFileMap;  // date -> absolute file path
    
    // Filter state
    QString currentSearchText;
    int currentDateFilter;  // 0=Last 7 Days, 1=Last 30 Days, 2=All
    
    // Weekly stats
    long long totalFocusTimeMs;
    int averageEfficiency;
};

#endif // HISTORYPAGE_H
