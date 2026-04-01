#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H

#include <QSlider>
#include <QPainter>

class CustomSlider : public QSlider
{
    Q_OBJECT
public:
    explicit CustomSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawGroove(QPainter &painter, const QRect &rect);
    void drawHandle(QPainter &painter, int handlePos);
};

#endif // CUSTOMSLIDER_H
