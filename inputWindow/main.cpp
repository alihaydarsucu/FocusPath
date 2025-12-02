#include <QApplication>
#include "inputwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    InputWindow window;
    window.show();

    return app.exec();
}

