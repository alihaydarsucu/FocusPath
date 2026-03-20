#ifndef FOCUSCARD_H
#define FOCUSCARD_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QEvent>
#include <QPixmap>

class FocusCard : public QFrame {
    Q_OBJECT

public:
    explicit FocusCard(const QString &title, const QString &description,
                       const QString &iconPath, const QString &statusText = "",
                       int width = 360, int height = 220, // Boyutlar buraya eklendi
                       QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupUI(int width, int height);
    void setData(const QString &title, const QString &description,
                 const QString &iconPath, const QString &statusText);

    QLabel *titleLabel;
    QLabel *descLabel;
    QLabel *iconLabel;
    QLabel *statusBadge;
    QFrame *iconCircle;
};

#endif
