#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //NewProcesses *np = new NewProcesses(&MainWindow);

    return a.exec();
}
