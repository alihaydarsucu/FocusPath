#include "MainWindow.h"

#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Keep the default text font first and add emoji-capable fallbacks to avoid glyph load errors.
    QFont emojiAware = a.font();
    emojiAware.setFamilies({emojiAware.family(), "Noto Color Emoji", "Apple Color Emoji", "Segoe UI Emoji"});
    a.setFont(emojiAware);

    MainWindow w;
    w.show();
    return a.exec();
}
