#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "includes/chunkhandler.h"
#include "includes/launcher.h"
#include "includes/packethandler.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    setWindowIcon(QIcon("logo.png"));
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    std::cout<<"------"<<"Closing MainWindow thread"<<"------"<<std::endl;
    delete ui;
}

void MainWindow::getNewConnection(sockaddr_in newConnection, PacketHandler* packetHandler)
{
    std::cout<<"New connection from:"<< ntohs(newConnection.sin_port)<<std::endl;
    ConnectionName connectionName = {inet_ntoa(newConnection.sin_addr),QString::number(ntohs(newConnection.sin_port))};
    ui->comboBox->addItem(connectionName.ip+":"+connectionName.port,connectionsCounter++);
    ui->comboBox->setItemData(ui->comboBox->count()-1,connectionName.ip+":"+connectionName.port,Qt::ToolTipRole);
    ui->comboBox->setItemIcon(ui->comboBox->count()-1,QIcon(":/green-dot.png"));

    initTraceWindow(connectionName);
    packetHandler->start();
}

void MainWindow::changeClientStatus(sockaddr_in client, tUINT32 status)
{
    QString clientName = inet_ntoa(client.sin_addr);
    clientName.push_back(":"+QString::number(ntohs(client.sin_port)));
    for(int i =0; i<ui->comboBox->count();i++){
        if(ui->comboBox->itemData(i,Qt::ToolTipRole)==clientName){
            if(status==OFFLINE){
                ui->comboBox->setItemIcon(i,QIcon(":/red-dot.png"));
                traceWindows.at(i)->setConnectionStatus(0);
            } else if(status==UNKNOWN_CONNECTION_STATUS){
                ui->comboBox->setItemIcon(i,QIcon(":/yellow-dot.png"));
                traceWindows.at(i)->setConnectionStatus(1);
            } else if(status==ONLINE){
                ui->comboBox->setItemIcon(i,QIcon(":/green-dot.png"));
                traceWindows.at(i)->setConnectionStatus(2);
            }
            return;
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    tUINT32 index = ui->comboBox->currentData().toString().toInt();

    if(ui->comboBox->currentText()==""){
        return;
    }

    if(traceWindows.at(index)->isVisible()){
        traceWindows.at(index)->raise();
        return;
    }

    traceWindows.at(index)->show();
}



TraceWindow* MainWindow::initTraceWindow(ConnectionName connectionName)
{
    traceWindow = new TraceWindow(connectionName,config);
    traceWindow->setConnectionStatus(ONLINE);
    traceWindows.append(traceWindow);
    tUINT32 index = traceWindows.size()-1;
    launcher->clientsList->at(index).connectionThread->chunkHandler.setTraceWindow(traceWindow);

    if(ui->autoOpen->isChecked()){
        traceWindow->show();
    }

    traceWindow->setStyle(styleSheet);

    return traceWindow;
}

void MainWindow::on_actionHigh_Contrast_Black_triggered()
{
    styleSheet = "color: white; background-color: rgb(0,0,0);";
    this->setStyleSheet(styleSheet);
    for(int i = 0;i<traceWindows.size();i++){
        traceWindows.at(i)->setStyle(styleSheet);
    }
}

void MainWindow::on_actionLike_in_QT_triggered()
{
    styleSheet = "color: white; background-color: rgb(42,43,44);";
    this->setStyleSheet(styleSheet);
    for(int i = 0;i<traceWindows.length();i++){
        traceWindows.at(i)->setStyle(styleSheet);
    }
}

void MainWindow::on_actionWhite_triggered()
{
    styleSheet = "";
    this->setStyleSheet(styleSheet);
    for(int i = 0;i<traceWindows.size();i++){
        traceWindows.at(i)->setStyle(styleSheet);
    }
}


void MainWindow::on_openFile_clicked()
{
    //Чтение из файла
    QString fileName = QFileDialog::getOpenFileName(this);
    if(fileName==""){
        return;
    }

    ui->comboBox->addItem("File: "+fileName,connectionsCounter++);
    ui->comboBox->setItemData(ui->comboBox->count()-1,"File: "+fileName,Qt::ToolTipRole);
    ui->comboBox->setItemIcon(ui->comboBox->count()-1,QIcon(":/green-dot.png"));

    traceWindow = new TraceWindow({"File ",fileName},config);
    fileReader = new FileReader(fileName,traceWindow);
    traceWindow->setTraceAsObject(fileReader->chunkHandler.getTraceHandler());
    traceWindow->setConnectionStatus(ONLINE);
    traceWindows.append(traceWindow);

    if(ui->autoOpen->isChecked()){
        traceWindow->show();
    }

    traceWindow->setStyle(styleSheet);

    fileReader->start();
}

