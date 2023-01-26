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
#include <QPushButton>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum CONNECTION_STATUS{
    OFFLINE = 0,
    UNKNOWN_CONNECTION_STATUS,
    ONLINE,
    FILE_CONNECTION
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
    void setLauncher(Launcher* newLauncher);
private:
    ConnectionName connectionName;
    ConfigHandler* config;
    tUINT32 connectionsCounter = 0;
    QString comboBoxText;
    Ui::MainWindow* ui;
    FileReader* fileReader;
    TraceWindow* initTraceWindow(ConnectionName connectionName);
    QString styleSheet = "";

    void resizeEvent(QResizeEvent *event);
public slots:
    void getNewConnection(sockaddr_in client, PacketHandler* packetHandler);
    void changeClientStatus(sockaddr_in client, tUINT32 status);
    void showContextMenu(const QPoint &pos);
private slots:
    void onCloseConnectionClicked();

    void on_actionHigh_Contrast_Black_triggered();
    void on_actionLike_in_QT_triggered();
    void on_actionWhite_triggered();
    void on_connectionsTable_cellDoubleClicked(int row, int column);
    void on_actionOpen_File_triggered();
signals:
    void sendClientToDelete(tUINT32 clientNumber);
};
#endif // MAINWINDOW_H
