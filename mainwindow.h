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

class PacketHandler;
class Launcher;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TraceWindow* traceWindow;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Launcher* launcher;
    QList<TraceWindow*> traceWindows;
private:
    ConnectionName connectionName;
    ConfigHandler *config;
    tUINT32 connectionsCounter = 0;
    QString comboBoxText;
    Ui::MainWindow *ui;
    FileReader *fileReader;
    void InitTraceWindow(ConnectionName connectionName);
    QString styleSheet = "";

public slots:
    void GetNewConnection(sockaddr_in client, PacketHandler *packetHandler);
    void ChangeClientStatus(sockaddr_in client);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_actionHigh_Contrast_Black_triggered();
    void on_actionLike_in_QT_triggered();
    void on_actionWhite_triggered();
};
#endif // MAINWINDOW_H
