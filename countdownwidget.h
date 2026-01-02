#ifndef COUNTDOWNWIDGET_H
#define COUNTDOWNWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class CountdownWidget : public QWidget
{
    Q_OBJECT

public:
    // Constructor sadece dakika alıyor
    explicit CountdownWidget(int totalMinutes, QWidget *parent = nullptr);
    void setMinutes(int totalMinutes);
    void stopTimer();  // Public method to stop timer
private slots:
    void updateCountdown();
signals:
    void timeoutReached();
private:
    void updateLabel();

    QLabel *label;
    QTimer *timer;
    int m_hours;
    int m_minutes;
    int m_seconds;  // Add seconds
};

#endif // COUNTDOWNWIDGET_H
