#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "winsock2.h"
#include <iostream>
#include "includes/tracewindow.h"
#include <QFileDialog>
#include "includes/FileReader/filereader.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Launcher;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TraceWindow* traceWindow;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Launcher* launcher;
private:
    QString comboBoxText;
    Ui::MainWindow *ui;
    FileReader *fileReader;
    void InitTraceWindow();
public slots:
    void GetNewConnection(sockaddr_in client);
    void ChangeClientStatus(sockaddr_in client);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};
#endif // MAINWINDOW_H
