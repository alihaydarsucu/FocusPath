#include "outputwindow.h"

#include "workflow.h"
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QStackedWidget>


OutputWindow::OutputWindow(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);


    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);

    QLabel *title = new QLabel("FOCUS PATH", this);
    title->setStyleSheet("font-size:20px; font-weight:600;");
    topLayout->addSpacing(10);
    topLayout->addWidget(title);

    nameWorkflow = new QLabel("-", this);
    topLayout->addWidget(nameWorkflow);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background:#F8F9FA;");



    QStackedWidget *stacked = new QStackedWidget(this);

    workflowPage = new WorkflowPage();
    ResultPage *resultPage = new ResultPage();

    stacked->addWidget(workflowPage); // index 0
    stacked->addWidget(resultPage);   // index 1

    connect(workflowPage, &WorkflowPage::workflowCompleted, [=]() {
        stacked->setCurrentWidget(resultPage);
    });

    connect(workflowPage, &WorkflowPage::workflowCompleted,
            this, [this]() {
                emit workflowComplated();
            });

    connect(resultPage, &ResultPage::backRequested, [=]() {
        emit backToDashboardRequested();
    });
    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(line);
    mainLayout->addWidget(stacked,1);

}
void OutputWindow::onWorkflowReady(Workflow &workflow)
{
    this->workflow = &workflow;
    workflowPage->getWorkflow(workflow);
    nameWorkflow->setText(
        QString::fromStdString(workflow.getName())
        );

    qDebug() << workflow.getName().c_str();
    qInfo() << workflow.getDuration();
    qInfo() << "onWorkflowReady called";

}


