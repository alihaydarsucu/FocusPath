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

    // Draw left part (mavi gradient - seçili)
    QLinearGradient gradient(grooveRect.left(), grooveRect.top(), grooveRect.right(), grooveRect.top());
    gradient.setColorAt(0, QColor(3, 136, 231));      // Cyan-Blue
    gradient.setColorAt(0.5, QColor(0, 176, 240));    // Light Blue
    gradient.setColorAt(1, QColor(0, 120, 212));      // Deep Blue
    painter.fillRect(grooveRect.left(), grooveRect.top(), pos - grooveRect.left() + handleWidth / 2, grooveRect.height(), gradient);

    // Draw right part (açık gri - seçilmemiş)
    painter.fillRect(pos + handleWidth / 2, grooveRect.top(), grooveRect.right() - (pos + handleWidth / 2), grooveRect.height(), QColor(240, 240, 245));

    // Draw handle (çember - ÖN PLANA)
    int handleY = grooveRect.top() + grooveRect.height() / 2;
    int handleX = pos + handleWidth / 2;
    
    // Handle gradient
    QRadialGradient handleGradient(QPoint(handleX, handleY), 10);
    handleGradient.setColorAt(0, QColor(255, 255, 255));           // White center
    handleGradient.setColorAt(1, QColor(230, 240, 255));           // Light blue edge
    painter.setBrush(handleGradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(handleX, handleY), 10, 10);
    
    // Blue border with shadow
    painter.setPen(QPen(QColor(0, 150, 230), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPoint(handleX, handleY), 10, 10);

    painter.end();
}
