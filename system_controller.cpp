#include "system_controller.hpp"

SystemController::SystemController(const QString& logDir, QObject* parent)
    : QObject(parent)
{
    handler = std::make_unique<SystemHandler>(logDir.toStdString());
}

SystemController::~SystemController() {
    handler->stop();
}

void SystemController::start() {
    if (!handler->isRunning()) {
        handler->start();
        emit runningChanged(true);
    }
}

void SystemController::stop() {
    if (handler->isRunning()) {
        handler->stop();
        emit runningChanged(false);
    }
}

bool SystemController::isRunning() const {
    return handler->isRunning();
}
