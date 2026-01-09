#include "customslider.h"
#include <QPainter>
#include <QStyleOptionSlider>
#include <cmath>

CustomSlider::CustomSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
{
}

void CustomSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QStyleOptionSlider opt;
    initStyleOption(&opt);

    // Groove rect
    QRect grooveRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    if (grooveRect.height() == 0) {
        grooveRect = rect().adjusted(0, (rect().height() - 6) / 2, 0, -(rect().height() - 6) / 2);
    }

    // Calculate handle position
    int handleWidth = 24;  // bigger handle
    int range = maximum() - minimum();
    int pos = 0;
    if (range != 0) {
        pos = grooveRect.left() + (value() - minimum()) * (grooveRect.width() - handleWidth) / range;
    } else {
        pos = grooveRect.left();
    }

    // Draw left part (mavi - seçili)
    painter.fillRect(grooveRect.left(), grooveRect.top(), pos - grooveRect.left() + handleWidth / 2, grooveRect.height(), QColor(0, 120, 212));

    // Draw right part (beyaz - seçilmemiş)
    painter.fillRect(pos + handleWidth / 2, grooveRect.top(), grooveRect.right() - (pos + handleWidth / 2), grooveRect.height(), Qt::white);

    // Draw handle (çember - ÖN PLANA)
    int handleY = grooveRect.top() + grooveRect.height() / 2;
    int handleX = pos + handleWidth / 2;
    
    // White circle inside
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(handleX, handleY), 10, 10);
    
    // Blue border
    painter.setPen(QPen(QColor(0, 120, 212), 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPoint(handleX, handleY), 10, 10);

    painter.end();
}
