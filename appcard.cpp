#include "appcard.h"

#include <QMouseEvent>
#include <QLabel>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

AppCard::AppCard(const QString& appName, const QIcon& icon, QWidget* parent)
    : QFrame(parent), m_appName(appName)
{
    setObjectName("AppCard");
    setCursor(Qt::PointingHandCursor);
    setFixedHeight(64);


    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_StyledBackground, true);

    QLabel* iconLabel = new QLabel(this);
    iconLabel->setPixmap(icon.pixmap(32, 32));
    iconLabel->setFixedSize(32, 32);
    iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    iconLabel->setStyleSheet(
        "background: transparent;"
        "border: none;"
        );


    QLabel* nameLabel = new QLabel(appName, this);
    nameLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    nameLabel->setStyleSheet(
        "background: transparent;"
        "border: none;"
        "font-size: 14px;"
        "color: #000000;"
        );


    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 8, 14, 8);
    layout->setSpacing(12);
    layout->addWidget(iconLabel);
    layout->addWidget(nameLabel);
    layout->addStretch();


    setStyleSheet(R"(
        QFrame#AppCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #FFFFFF, stop:1 #F8FBFD);
            border-radius: 12px;
            border: 1px solid #E8F0F7;
        }
        QFrame#AppCard:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #F0F8FF, stop:1 #E0F7FF);
            border: 2px solid #0288D1;
        }
    )");

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);

    qDebug() << "[AppCard] Created" << appName;
}

QString AppCard::appName() const
{
    return m_appName;
}

void AppCard::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "[AppCard] Clicked" << m_appName << "button" << event->button();
    emit clicked(m_appName);


    event->accept();
}
