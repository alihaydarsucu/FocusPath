#include "inputwindow.h"
#include "outputwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>

InputWindow::InputWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("FocusPath - Session Setup");
    resize(450, 400);

    auto *mainLayout = new QVBoxLayout(this);

    // --- Workflow App Input ---
    auto *workflowLabel = new QLabel("Workflow Applications:");
    mainLayout->addWidget(workflowLabel);

    auto *appLayout = new QHBoxLayout();
    appInput = new QLineEdit();
    QPushButton *addBtn = new QPushButton("Add");
    appLayout->addWidget(appInput);
    appLayout->addWidget(addBtn);

    mainLayout->addLayout(appLayout);

    workflowList = new QListWidget();
    mainLayout->addWidget(workflowList);

    connect(addBtn, &QPushButton::clicked, this, &InputWindow::addWorkflowApp);

    // --- Duration Selection ---
    auto *durationLabel = new QLabel("Session Duration:");
    mainLayout->addWidget(durationLabel);

    durationCombo = new QComboBox();
    durationCombo->addItems({"30 minutes", "1 hour", "2 hours", "3 hours", "Custom"});
    mainLayout->addWidget(durationCombo);

    // --- Template Selection ---
    auto *templateLabel = new QLabel("Workflow Templates:");
    mainLayout->addWidget(templateLabel);

    templateCombo = new QComboBox();
    templateCombo->addItems({"Deep Work", "Coding Session", "Design Session"});
    mainLayout->addWidget(templateCombo);

    // --- Start Session ---
    QPushButton *startBtn = new QPushButton("Start Session");
    mainLayout->addWidget(startBtn);

    connect(startBtn, &QPushButton::clicked, this, &InputWindow::startSession);
}

void InputWindow::addWorkflowApp()
{
    QString appName = appInput->text();
    if (!appName.isEmpty()) {
        workflowList->addItem(appName);
        appInput->clear();
    }
}

void InputWindow::startSession()
{
    // Burada session backend başlatılacak.
    // Şimdilik direkt sonuç ekranına geçiyoruz.

    OutputWindow *output = new OutputWindow();
    output->show();
    this->close();
}
