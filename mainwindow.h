
#pragma once

#include "inputwindow.h"
#include "outputwindow.h"
#include "workflow.h"
#include <QMainWindow>
#include <QStackedWidget>
#include "system_controller.hpp"

class InputWindow;
class OutputWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void goToOutput(Workflow &workflow);
    void startTracking();
    void stopTracking();


private:
    QStackedWidget *stack;
    InputWindow *input;
    OutputWindow *output;
    SystemController* controller;
};
