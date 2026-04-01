#include "CountdownWidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QString>

CountdownWidget::CountdownWidget(int totalMinutes, QWidget *parent)
    : QWidget(parent), m_seconds(0)
{
    m_hours = totalMinutes / 60;
    m_minutes = totalMinutes % 60;

    qDebug() << "[CountdownWidget] Init with" << totalMinutes << "minutes";

    setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FFFFFF, stop:1 #F8FBFD); "
        "  border-radius: 15px; "
        "  border: 2px solid #E8F0F7; "
        "}"
    );


    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: #0288D1; font-size: 96px; font-weight: bold; background: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->setContentsMargins(20, 20, 20, 20);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CountdownWidget::updateCountdown);
    timer->start(1000); // Update every second

    updateLabel();
}

void CountdownWidget::setMinutes(int totalMinutes)
{
    m_hours = totalMinutes / 60;
    m_minutes = totalMinutes % 60;
    m_seconds = 0;

    qDebug() << "[CountdownWidget] Minutes set to" << totalMinutes;
    if (timer) {
        timer->stop();
        timer->start(1000);  // Restart timer
    }
    updateLabel();
}

void CountdownWidget::updateCountdown()
{
    qDebug() << "[CountdownWidget] Tick" << m_hours << "h" << m_minutes << "m" << m_seconds << "s";

    if (m_minutes == 0 && m_hours == 0 && m_seconds == 0) {
        timer->stop();
        label->setText("00:00:00");
        emit timeoutReached();
        return;
    }

    if (m_seconds == 0) {
        if (m_minutes == 0 && m_hours > 0) {
            m_hours--;
            m_minutes = 59;
            m_seconds = 59;
        } else if (m_minutes > 0) {
            m_minutes--;
            m_seconds = 59;
        }
    } else {
        m_seconds--;
    }

    updateLabel();
}

void CountdownWidget::updateLabel()
{
    label->setText(QString("%1:%2:%3")
                       .arg(m_hours, 2, 10, QChar('0'))
                       .arg(m_minutes, 2, 10, QChar('0'))
                       .arg(m_seconds, 2, 10, QChar('0')));
}

void CountdownWidget::stopTimer()
{
    if (timer) {
        qDebug() << "[CountdownWidget] Timer stopped at" << m_hours << ":" << m_minutes << ":" << m_seconds;
        timer->stop();
    }
}
