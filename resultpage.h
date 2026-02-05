#ifndef RESULTPAGE_H
#define RESULTPAGE_H

#include <QWidget>
#include <QLabel>
#include <vector>
#include <string>

#include "app_usage_analyzer.hpp"
#include "attention_analyzer.hpp"
#include "GraphicGenerator_header_and_structs.h"

#include <QtSvgWidgets/QSvgWidget>

class ResultPage : public QWidget
{
    Q_OBJECT

public:
    explicit ResultPage(QWidget *parent = nullptr);

    void showResults(const AppUsageAnalyzer::AppUsageResult &usage,
                     const AttentionAnalyzer::AttentionStats &attention,
                     const QString &logFile,
                     const std::vector<std::string> &trackedApps,
                     const SessionCharts &charts);

    void showMessage(const QString &message);
    void setWorkflowName(const QString &name);

signals:
    void backRequested();

private:
    void setupUI();
    static QString formatMs(long long ms);

    QLabel *workflowNameLabel{};
    QLabel *statusLabel{};
    QLabel *focusLabel{};
    QLabel *unfocusLabel{};
    QLabel *focusPercentLabel{};
    QLabel *distractPercentLabel{};
    QLabel *distractionCountLabel{};
    QLabel *longestFocusLabel{};
    QLabel *longestDistractLabel{};
    QLabel *appsLabel{};
    QLabel *logPathLabel{};

    QSvgWidget *usageChart{};
    QSvgWidget *timelineChart{};
    QSvgWidget *pieChart{};
    QSvgWidget *statsChart{};
};

#endif // RESULTPAGE_H
