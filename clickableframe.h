#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QEnterEvent>

class ClickableFrame : public QFrame {
    Q_OBJECT

public:
    explicit ClickableFrame(
        const QString& title,
        const QString& iconPath,
        QWidget* parent = nullptr
        );

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QLabel* iconLabel;
    QLabel* titleLabel;

    void setNormalStyle();
    void setHoverStyle();
};

#endif
