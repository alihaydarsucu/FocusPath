
#include "MainWindow.h"
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Pencere özelliklerini ayarla
    setWindowTitle("FocusPath");
    setMinimumSize(1000, 600);
    resize(1200, 800);
    
    // Modern gradient arkaplan
    setStyleSheet(
        "QMainWindow { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FAFBFF, stop:0.5 #F0F6FF, stop:1 #E8F4FF); "
        "}"
    );
    
    // Pencere bayraklarını kontrol et (maximize/minimize/close butonları)
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

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

        connect(input, &InputWindow::reportRequested,
                this, [=](const Workflow &wf) {
                    stack->setCurrentWidget(output);
                    output->showReportForWorkflow(wf);
                });

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
