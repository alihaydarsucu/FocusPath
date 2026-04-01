#ifndef TEMPLATESPAGE_H
#define TEMPLATESPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QHash>
#include "Workflow.h"
#include "WorkflowCard.h"

class TemplatesPage : public QWidget
{
    Q_OBJECT

public:
    explicit TemplatesPage(QWidget *parent = nullptr);
    void refreshWorkflows();

signals:
    void startWorkflow(Workflow &workflow);

private slots:
    void onSearchTextChanged(const QString &text);
    void onSortChanged(int index);
    void onWorkflowStartRequested(const Workflow &workflow);
    void onWorkflowDeleteRequested(const Workflow &workflow);

private:
    void setupUI();
    void loadFavoriteWorkflows();
    void displayWorkflows();
    void filterAndSortWorkflows();

    // UI Components
    QLineEdit *searchBox;
    QComboBox *sortCombo;
    QScrollArea *scrollArea;
    QWidget *cardsContainer;
    QGridLayout *cardsLayout;

    // Data
    QVector<Workflow> allWorkflows;
    QVector<Workflow> filteredWorkflows;
    QHash<QString, QString> workflowFileMap; // date -> absolute file path
    QString currentSearchText;
    int currentSortIndex;
};

#endif // TEMPLATESPAGE_H
