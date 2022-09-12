#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "includes/chunkhandler.h"
#include "includes/launcher.h"

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

void MainWindow::on_pushButton_clicked()
{
    //Нужно написать сеттер для chunkHandlera, который получает traceWindow и выставялет windowOpened как true
    //Чтобы было по красоте
    for(int i = 0;i<10;i++)
    {
        //Ищем клиента из комбобокса
        if(ui->comboBox->currentText().toInt()==ntohs(launcher->clientsList[i].clientIp.sin_port))
        {
            traceWindow = new TraceWindow();
//            //Так делать нельзя, надо будет переделать
            launcher->clientsList[i].connectionThread->chunkHandler.setTraceWindow(traceWindow);
            traceWindow->show();
        }
    }

}

