#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <memory>

#include "system_handler.hpp"

class SystemController : public QObject {
    Q_OBJECT

public:
    explicit SystemController(const QString& logDir, QObject* parent = nullptr);

    bool isRunning() const;

public slots:
    void start();
    void stop();

private:
    std::unique_ptr<SystemHandler> handler;
    QTimer* timer;
};
