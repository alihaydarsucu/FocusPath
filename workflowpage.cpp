#include "workflowpage.h"
#include "workflow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

WorkflowPage::WorkflowPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &WorkflowPage::updateCountdown);
}

void WorkflowPage::setupUI()
{
    // Modern gradient background
    this->setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FAFBFF, stop:0.5 #F0F6FF, stop:1 #E8F4FF); "
        "}"
    );

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Workflow Page", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #0288D1;");

    // Countdown label
    countdownLabel = new QLabel("--:--", this);
    countdownLabel->setAlignment(Qt::AlignCenter);
    countdownLabel->setStyleSheet(
        "font-size: 48px; "
        "font-weight: bold; "
        "color: #0288D1; "
        "padding: 20px; "
        "background: transparent;"
    );

    QPushButton *stopBtn = new QPushButton("Stop Workflow", this);
    stopBtn->setStyleSheet(
        "QPushButton { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #E74C3C, stop:1 #C0392B); "
        "  color: white; "
        "  font-size: 16px; "
        "  font-weight: bold; "
        "  padding: 12px 24px; "
        "  border-radius: 8px; "
        "  border: none; "
        "} "
        "QPushButton:hover { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #C0392B, stop:1 #A93226); "
        "}"
    );

    connect(stopBtn, &QPushButton::clicked, this, &WorkflowPage::onBackClicked);

    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(countdownLabel);
    layout->addStretch();
    layout->addWidget(stopBtn, 0, Qt::AlignCenter);
}

void WorkflowPage::getWorkflow(Workflow &workflow)
{
    this->workflow = &workflow;
    
    // Start countdown
    remainingSeconds = workflow.getDuration() * 60; // Convert minutes to seconds
    updateCountdown();
    countdownTimer->start(1000); // Update every second
}

void WorkflowPage::updateCountdown()
{
    if (remainingSeconds <= 0) {
        countdownTimer->stop();
        countdownLabel->setText("00:00");
        
        QMessageBox::information(this, "Workflow Completed", 
                                "Your workflow session has ended!");
        emit workflowCompleted();
        return;
    }
    
    int hours = remainingSeconds / 3600;
    int minutes = (remainingSeconds % 3600) / 60;
    int seconds = remainingSeconds % 60;
    
    QString timeText;
    if (hours > 0) {
        timeText = QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    } else {
        timeText = QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
    
    countdownLabel->setText(timeText);
    remainingSeconds--;
}

void WorkflowPage::onBackClicked()
{
    QMessageBox msg(this);
    msg.setWindowTitle("Confirm");
    msg.setText("Are you sure you want to stop this Workflow!!");
    msg.setInformativeText("Workflow session will end.");
    msg.setIcon(QMessageBox::Warning);

    QPushButton *yesBtn = msg.addButton("Yes", QMessageBox::AcceptRole);
    msg.addButton("Cancel", QMessageBox::RejectRole);

    msg.exec();

    if (msg.clickedButton() == yesBtn) {
        countdownTimer->stop();
        emit workflowCompleted();
    }
}
