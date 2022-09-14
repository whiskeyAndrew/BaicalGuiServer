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
    comboBoxText = inet_ntoa(newConnection.sin_addr);
    comboBoxText.push_back(":"+QString::number(ntohs(newConnection.sin_port)));
   // ui->comboBox->addItem(QString::number(ntohs(newConnection.sin_port)));
    ui->comboBox->addItem(comboBoxText);
}

void MainWindow::on_pushButton_clicked()
{
    //Говнокод, пофиксить надо потом
    QString tempComboBoxText = ui->comboBox->currentText();
    tempComboBoxText.remove(0,tempComboBoxText.length()-5);

    //Нужно написать сеттер для chunkHandlera, который получает traceWindow и выставялет windowOpened как true
    //Чтобы было по красоте
    for(int i = 0;i<10;i++)
    {
        //Ищем клиента из комбобокса
        if(tempComboBoxText==QString::number(ntohs(launcher->clientsList[i].clientIp.sin_port)))
        {
            traceWindow = new TraceWindow();
//            //Так делать нельзя, надо будет переделать
            launcher->clientsList[i].connectionThread->chunkHandler.setTraceWindow(traceWindow);
            traceWindow->show();
        }
    }

}

