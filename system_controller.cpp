#include "system_controller.hpp"

SystemController::SystemController(const QString& logDir, QObject* parent)
    : QObject(parent)
{
    handler = std::make_unique<SystemHandler>(logDir.toStdString());

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        handler->tick();
    });
}

void SystemController::start() {
    timer->start(200);
}

void SystemController::stop() {
    timer->stop();
}

bool SystemController::isRunning() const {
    return timer->isActive();
}

