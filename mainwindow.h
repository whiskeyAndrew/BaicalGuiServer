#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "winsock2.h"
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Launcher;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:

    Ui::MainWindow *ui;
public slots:
    void GetNewConnection(sockaddr_in client);
};
#endif // MAINWINDOW_H
