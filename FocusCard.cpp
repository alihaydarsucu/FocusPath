#include "FocusCard.h"

FocusCard::FocusCard(const QString &title, const QString &description,
                     const QString &iconPath, const QString &statusText,
                     int width, int height,
                     QWidget *parent) : QFrame(parent)
{
    setupUI(width, height);
    setData(title, description, iconPath, statusText);
    this->setCursor(Qt::PointingHandCursor);
}

void FocusCard::setupUI(int width, int height) {
    this->setObjectName("FocusCard");
    this->setMaximumSize(width, height);
    //this->setMaximumWidth(width);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    this->setStyleSheet(
        "#FocusCard { "
        "  background-color: #FFFFFF; "
        "  border-radius: 24px; "
        "  border: 1px solid #F0F0F0; "
        "}"
        );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setXOffset(0);
    shadow->setYOffset(10);
    shadow->setColor(QColor(0, 0, 0, 25));
    this->setGraphicsEffect(shadow);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(12);

    QHBoxLayout *topLayout = new QHBoxLayout();

    iconCircle = new QFrame();
    iconCircle->setFixedSize(48, 48);
    iconCircle->setStyleSheet("background-color: #EBF5F9; border-radius: 14px;");

    iconLabel = new QLabel(iconCircle);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("border: none; background: transparent;");

    QVBoxLayout *iconInnerLayout = new QVBoxLayout(iconCircle);
    iconInnerLayout->setContentsMargins(0,0,0,0);
    iconInnerLayout->addWidget(iconLabel);

    statusBadge = new QLabel();
    statusBadge->setAlignment(Qt::AlignCenter);
    statusBadge->setContentsMargins(10, 4, 10, 4);
    statusBadge->setStyleSheet(
        "background-color: #F0FAF5; color: #1A7F50; "
        "font-weight: bold; font-size: 10px; border-radius: 8px;"
        );

    topLayout->addWidget(iconCircle);
    topLayout->addStretch();
    topLayout->addWidget(statusBadge);

    titleLabel = new QLabel();
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #212529; background: transparent;");

    descLabel = new QLabel();
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("font-size: 13px; color: #6C757D; line-height: 1.4; background: transparent;");

    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(8);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(descLabel);
    mainLayout->addStretch();
}

void FocusCard::setData(const QString &title, const QString &description,
                        const QString &iconPath, const QString &status) {
    titleLabel->setText(title);
    descLabel->setText(description);


    QPixmap pix(iconPath);
    if (!pix.isNull()) {
        iconLabel->setPixmap(pix.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    if (status.isEmpty()) statusBadge->hide();
    else {
        statusBadge->setText(status.toUpper());
        statusBadge->show();
    }
}

void FocusCard::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) emit clicked();
    QFrame::mousePressEvent(event);
}

void FocusCard::enterEvent(QEnterEvent *event) {
    this->setStyleSheet(
        "#FocusCard { background-color: #FAFAFA; border-radius: 24px; border: 1px solid #006692; }"
        );
}

void FocusCard::leaveEvent(QEvent *event) {
    this->setStyleSheet(
        "#FocusCard { background-color: #FFFFFF; border-radius: 24px; border: 1px solid #F0F0F0; }"
        );
}
