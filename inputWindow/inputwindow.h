#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <QStringList>
#include <QListWidget>

class InputWindow : public QWidget
{
    Q_OBJECT

public:
    InputWindow(QWidget *parent = nullptr);

signals:

private slots:
    void addWorkflowApp();
    void startSession();

private:
    QLineEdit *appInput;
    QListWidget *workflowList;
    QComboBox *durationCombo;
    QComboBox *templateCombo;
};

#endif
