#include "inputwindow.h"

#include "clickablelabel.h"
#include "dashboardpage.h"
#include "workflowsetuppage.h"
#include "historypage.h"
#include "templatespage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QWidget>
#include <QPushButton>
#include <QLabel>

InputWindow::InputWindow(QWidget *parent) : QWidget(parent){


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);


    QWidget *topWidget = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);

    QLabel *title = new QLabel("FocusPath", this);
    title->setStyleSheet("font-size:20px; font-weight:600;");
    topLayout->addSpacing(10);
    topLayout->addWidget(title);
    topLayout->addStretch();

    dashboard = new ClickableLabel(this);
    workflowSetup = new ClickableLabel(this);
    history = new ClickableLabel(this);
    templates = new ClickableLabel(this);

    dashboard->setText("Dashboard");
    workflowSetup->setText("Workflow Setup");
    history->setText("History");
    templates->setText("Templates");

    QList<ClickableLabel*> menus {
        dashboard, workflowSetup, history, templates
    };

    for (auto *m : menus) {
        m->setStyleSheet("font-size:16px; font-weight:450; color:#5F6368;");
        topLayout->addWidget(m);
        topLayout->addStretch();

        connect(m, &ClickableLabel::clicked, this, [=]() {
            if (current) {
                current->setSelected(false);
                current->setStyleSheet(
                    "font-size:16px; font-weight:450; color:#5F6368;"
                    );
            }

            m->setSelected(true);
            m->setStyleSheet(
                "font-size:16px; font-weight:600; color:#0078d4;"
                );
            current = m;
        });
    }


    dashboard->setSelected(true);
    dashboard->setStyleSheet(
        "font-size:16px; font-weight:600; color:#0078d4;"
        );
    current = dashboard;


    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background:#F8F9FA;");


    stack = new QStackedWidget(this);
    stack->setStyleSheet("background:#F0F0F0;");

    DashboardPage *dashboardPage = new DashboardPage(this);
    WorkflowSetupPage *workflowPage  = new WorkflowSetupPage(this);
    HistoryPage *historyPage   = new HistoryPage(this);
    TemplatesPage *templatesPage = new TemplatesPage(this);

    stack->addWidget(dashboardPage);
    stack->addWidget(workflowPage);
    stack->addWidget(historyPage);
    stack->addWidget(templatesPage);

    stack->setCurrentIndex(0);


    connect(dashboard, &ClickableLabel::clicked, this, [=]() {
        stack->setCurrentIndex(0);
    });

    connect(workflowSetup, &ClickableLabel::clicked, this, [=]() {
        stack->setCurrentIndex(1);
    });

    connect(history, &ClickableLabel::clicked, this, [=]() {
        stack->setCurrentIndex(2);
    });

    connect(templates, &ClickableLabel::clicked, this, [=]() {
        stack->setCurrentIndex(3);
    });



    connect(workflowPage, &WorkflowSetupPage::startWorkflow,
            this, [=](Workflow &workflow) {

                emit requestOutputPage(workflow);   // 🔥 VERİ BURADAN YUKARI ÇIKIYOR
                emit startRequested();     // sayfa geçişi
            });

    // Connect templates page start workflow signal
    connect(templatesPage, &TemplatesPage::startWorkflow,
            this, [=](Workflow &workflow) {
                emit requestOutputPage(workflow);
                emit startRequested();
            });

    // Connect history page start workflow signal
    connect(historyPage, &HistoryPage::startWorkflow,
            this, [=](Workflow &workflow) {
                emit requestOutputPage(workflow);
                emit startRequested();
            });

    connect(workflowPage, &WorkflowSetupPage::backToDashboard, this, [=]() {
        stack->setCurrentIndex(0);
        if (current != dashboard) {
            current->setSelected(false);
            current->setStyleSheet("font-size:16px; font-weight:450; color:#5F6368;");
            dashboard->setSelected(true);
            dashboard->setStyleSheet("font-size:16px; font-weight:600; color:#0078d4;");
            current = dashboard;
        }
    });


    connect(dashboardPage, &DashboardPage::pageRequested,
            this, [=](int index) {
                stack->setCurrentIndex(index);


                ClickableLabel* menu = nullptr;

                switch (index) {
                case 0: menu = dashboard; break;
                case 1: menu = workflowSetup; break;
                case 2: menu = history; break;
                case 3: menu = templates; break;
                }

                if (menu && current != menu) {
                    current->setSelected(false);
                    current->setStyleSheet("font-size:16px; font-weight:450; color:#5F6368;");
                    menu->setSelected(true);
                    menu->setStyleSheet("font-size:16px; font-weight:600; color:#0078d4;");
                    current = menu;
                }
            });

    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(line);
    mainLayout->addWidget(stack, 1);
}

void InputWindow::showDashboard()
{
    stack->setCurrentIndex(0);
    if (current != dashboard) {
        current->setSelected(false);
        current->setStyleSheet("font-size:16px; font-weight:450; color:#5F6368;");
        dashboard->setSelected(true);
        dashboard->setStyleSheet("font-size:16px; font-weight:600; color:#0078d4;");
        current = dashboard;
    }
}
