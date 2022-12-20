#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "winsock2.h"
#include <iostream>
#include "includes/TraceWindow/tracewindow.h"
#include <QFileDialog>
#include "includes/FileReader/filereader.h"
#include <QMessageBox>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum CONNECTION_STATUS{
    OFFLINE = 0,
    UNKNOWN_CONNECTION_STATUS,
    ONLINE
};

class PacketHandler;
class Launcher;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TraceWindow* traceWindow;
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    Launcher* launcher;
    QList<TraceWindow*> traceWindows;
private:
    ConnectionName connectionName;
    ConfigHandler* config;
    tUINT32 connectionsCounter = 0;
    QString comboBoxText;
    Ui::MainWindow* ui;
    FileReader* fileReader;
    void initTraceWindow(ConnectionName connectionName);
    QString styleSheet = "";

public slots:
    void getNewConnection(sockaddr_in client, PacketHandler* packetHandler);
    void changeClientStatus(sockaddr_in client, tUINT32 status);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_actionHigh_Contrast_Black_triggered();
    void on_actionLike_in_QT_triggered();
    void on_actionWhite_triggered();
};
#endif // MAINWINDOW_H
