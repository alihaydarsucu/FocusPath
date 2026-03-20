#include "clickableframe.h"
#include <QIcon>
#include <QPainter>
#include <QCursor>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

ClickableFrame::ClickableFrame(
    const QString& title,
    const QString& iconPath,
    QWidget* parent
    )
    : QFrame(parent)
{
    setCursor(Qt::PointingHandCursor);
    setFixedHeight(56);
    setFrameShape(QFrame::NoFrame);

    iconLabel  = new QLabel(this);
    titleLabel = new QLabel(title, this);


    QIcon icon(iconPath);
    iconLabel->setFixedSize(24, 24);
    iconLabel->setPixmap(icon.pixmap(24, 24));
    iconLabel->setAlignment(Qt::AlignCenter);



    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 10, 16, 10);
    layout->setSpacing(10);

    layout->addStretch();
    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addStretch();

    setLayout(layout);


    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);

    setNormalStyle();

    qDebug() << "[ClickableFrame] Created" << title << "icon" << iconPath;
}

void ClickableFrame::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug() << "[ClickableFrame] Clicked" << titleLabel->text();
        emit clicked();
    }

    QFrame::mousePressEvent(event);
}

void ClickableFrame::enterEvent(QEvent*)
{
    qDebug() << "[ClickableFrame] Hover in" << titleLabel->text();
    setHoverStyle();
}

void ClickableFrame::leaveEvent(QEvent*)
{
    qDebug() << "[ClickableFrame] Hover out" << titleLabel->text();
    setNormalStyle();
}


void ClickableFrame::setNormalStyle()
{
    setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #FFFFFF, stop:1 #F8FBFD);
            border: 1px solid #E8F0F7;
            border-radius: 14px;
        }
        QLabel {
            color: #111111;
            font-size: 14px;
            font-weight: 500;
            background: transparent;
        }
    )");
}


void ClickableFrame::setHoverStyle()
{
    setStyleSheet(R"(
        QFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #F0F8FF, stop:1 #E0F7FF);
            border: 2px solid #0288D1;
            border-radius: 14px;
        }
        QLabel {
            color: #000000;
            background: transparent;
        }
    )");
}
