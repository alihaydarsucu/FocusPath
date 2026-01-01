#include "dashboardpage.h"
#include <QGridLayout>
#include "FocusCard.h"
#include "imagecard.h"


DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *bottomLayout = new QGridLayout(this);
    bottomLayout->setSpacing(40);
    bottomLayout->setContentsMargins(170, 50, 170, 50);

    FocusCard *card1 = new FocusCard(
        "Start New Session",
        "Configure workflow apps and timer to begin your deep work block.",
        ":/pictures/icons8-play-48.png",
        "READY",
        440,180
        );

    FocusCard *card2 = new FocusCard(
        "View History",
        "Check your stats.",
        ":/pictures/history.png",
        "",
        400,180
        );

    FocusCard *card3 = new FocusCard(
        "Templates     ",
        "Saved workflows.",
        ":/pictures/lace.png",
        "",
        200,180
        );
    QObject::connect(card1, &FocusCard::clicked, this, [=]() {
        emit pageRequested(1); // Workflow Setup
    });

    QObject::connect(card2, &FocusCard::clicked, this, [=]() {
        emit pageRequested(2); // History
    });

    QObject::connect(card3, &FocusCard::clicked, this, [=]() {
        emit pageRequested(3); // Templates
    });



    ImageCard *card = new ImageCard();
    card->setMinimumSize(600, 400);

    bottomLayout->addWidget(card1,1,0,1,2);
    bottomLayout->addWidget(card2,2,0,1,1);
    bottomLayout->addWidget(card3,2,1,1,1);
    bottomLayout->addWidget(card,0,3,4,2);

    bottomLayout->setRowStretch(2,1);
    bottomLayout->setColumnStretch(2,1);
}
