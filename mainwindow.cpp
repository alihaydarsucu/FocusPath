
#include "mainwindow.h"
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    stack = new QStackedWidget(this);

    input  = new InputWindow;
    output = new OutputWindow;

    stack->addWidget(input);
    stack->addWidget(output);

    setCentralWidget(stack);

    // Create logs directory if it doesn't exist
    QString logsDir = QCoreApplication::applicationDirPath() + "/logs";
    QDir dir;
    if (!dir.exists(logsDir)) {
        dir.mkpath(logsDir);
    }

    controller = new SystemController(logsDir + "/", this);

    connect(input, &InputWindow::requestOutputPage,
            this, &MainWindow::goToOutput);

    connect(input, &InputWindow::requestOutputPage,
            output, &OutputWindow::onWorkflowReady);

    connect(input, &InputWindow::startRequested,
            this, &MainWindow::startTracking);

    connect(output, &OutputWindow::workflowComplated,
            this, &MainWindow::stopTracking);

    connect(output, &OutputWindow::backToDashboardRequested, this, [=]() {
        stack->setCurrentWidget(input);
        input->showDashboard();
    });

}

void MainWindow::goToOutput(Workflow &workflow)
{
    stack->setCurrentWidget(output);
}
void MainWindow::startTracking()
{
    controller->start();
}
void MainWindow::stopTracking()
{
    controller->stop();
}
