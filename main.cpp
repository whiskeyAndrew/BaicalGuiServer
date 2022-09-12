#include "includes/launcher.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<sockaddr_in>("sockaddr_in");
    qRegisterMetaType<TraceLineData>("TraceLineData");
    qRegisterMetaType<tUINT32>("tUINT32");

    QApplication a(argc, argv);
    MainWindow mainWindow;
    Launcher launcher;
    mainWindow.launcher = &launcher;
    launcher.SetMainWindow(&mainWindow);
    launcher.start();

    mainWindow.show();

    return a.exec();
}

