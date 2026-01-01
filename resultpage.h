#ifndef RESULTPAGE_H
#define RESULTPAGE_H

#include <QWidget>

class ResultPage : public QWidget
{
    Q_OBJECT

public:
    explicit ResultPage(QWidget *parent = nullptr);

signals:

    void backRequested();

private:
    void setupUI();
};

#endif // RESULTPAGE_H
