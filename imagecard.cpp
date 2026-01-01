#include "imagecard.h"
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QRegion>
#include <QPixmap>
#include <QPainterPath>

ImageCard::ImageCard(QWidget *parent)
    : QFrame(parent)
{
    setObjectName("imageCard");
    setMinimumSize(260, 180);

    // Style
    setStyleSheet(
        "#imageCard {"
        "background-color: white;"
        "border-radius: 16px;"
        "}"
        );


    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(shadow);


    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);


    imageLabel = new QLabel(this);
    imageLabel->setScaledContents(true);
    imageLabel->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );

    imageLabel->setPixmap(QPixmap(":/pictures/suluboya1.PNG"));
    layout->addWidget(imageLabel);


    }

void ImageCard::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    QPainterPath path;
    path.addRoundedRect(rect(), 16, 16);

    QRegion region(path.toFillPolygon().toPolygon());
    setMask(region);
}


