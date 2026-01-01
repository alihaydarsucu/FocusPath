#pragma once
#include <QFrame>
#include <QLabel>

    class ImageCard : public QFrame
{
    Q_OBJECT

public:
    explicit ImageCard(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *imageLabel;
};
