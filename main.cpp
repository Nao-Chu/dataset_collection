#include "widget.h"

#include "log.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    qInstallMessageHandler(onMessageOutput);

    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
