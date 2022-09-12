#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "winsock2.h"
#include <iostream>
#include "includes/tracewindow.h"

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

    Ui::MainWindow *ui;
public slots:
    void GetNewConnection(sockaddr_in client);
private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
