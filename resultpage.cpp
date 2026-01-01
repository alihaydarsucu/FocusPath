#include "resultpage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

ResultPage::ResultPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ResultPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Result Page", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold;");

    QPushButton *backBtn = new QPushButton("Back to Workflow", this);

    connect(backBtn, &QPushButton::clicked, this, &ResultPage::backRequested);

    layout->addWidget(title);
    layout->addStretch();
    layout->addWidget(backBtn);
}
