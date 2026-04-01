#include "ClickableLabel.h"
#include <QPainter>

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
{
    setCursor(Qt::PointingHandCursor);
    setContentsMargins(6, 6, 6, 6);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
    QLabel::mousePressEvent(event);
}

void ClickableLabel::setSelected(bool s)
{
    if (m_selected == s) return;
    m_selected = s;
    update();
}

void ClickableLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    if (!m_selected) return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Modern cyan-blue color with gradient effect
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0, QColor(224, 247, 255));
    gradient.setColorAt(0.5, QColor(2, 136, 209));
    gradient.setColorAt(1, QColor(224, 247, 255));

    p.setPen(QPen(QColor(2, 136, 209), 3));
    p.setBrush(gradient);

    int y = height() - 2;
    p.drawLine(0, y, width(), y);
}
