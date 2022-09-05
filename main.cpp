#include "includes/launcher.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<sockaddr_in>("sockaddr_in");
    QApplication a(argc, argv);
    MainWindow mainWindow;
    Launcher launcher;

    launcher.SetMainWindow(&mainWindow);
    launcher.start();

    mainWindow.show();

    return a.exec();
}
