#pragma once
#include <QObject>
#include <memory>
#include "system_handler.hpp"

class SystemController : public QObject {
    Q_OBJECT
public:
    explicit SystemController(const QString& logDir, QObject* parent = nullptr);
    ~SystemController();

public slots:
    void start();
    void stop();
    bool isRunning() const;

signals:
    void runningChanged(bool);

private:
    std::unique_ptr<SystemHandler> handler;
};
