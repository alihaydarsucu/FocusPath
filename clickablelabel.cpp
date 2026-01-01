#include "clickablelabel.h"
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

    QColor accent("#0078d4");
    p.setPen(QPen(accent, 2));

    int y = height() - 1 ;
    p.drawLine(0, y, width(), y);
}
