#ifndef APPCARD_H
#define APPCARD_H

#include <QFrame>
#include <QString>
#include <QIcon>

class AppCard : public QFrame
{
    Q_OBJECT
public:
    explicit AppCard(const QString& appName, const QIcon& icon, QWidget* parent = nullptr);

    QString appName() const;

signals:
    void clicked(const QString& name);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QString m_appName;
};

#endif // APPCARD_H
