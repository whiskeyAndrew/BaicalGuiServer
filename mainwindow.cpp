#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetNewConnection(sockaddr_in newConnection)
{
    std::cout<<"New connection from:"<< ntohs(newConnection.sin_port)<<std::endl;
    ui->comboBox->addItem(QString::number(ntohs(newConnection.sin_port)));
}
