#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "includes/chunkhandler.h"
#include "includes/launcher.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    setWindowIcon(QIcon("logo.png"));
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
    comboBoxText.push_front("🟢");
    // ui->comboBox->addItem(QString::number(ntohs(newConnection.sin_port)));
    ui->comboBox->addItem(comboBoxText);
    bool checkBoxState = ui->checkBox->isChecked();
    if(checkBoxState==true)
    {
        InitTraceWindow();
    }
}

void MainWindow::ChangeClientStatus(sockaddr_in client)
{
    QString clientName = inet_ntoa(client.sin_addr);
    clientName.push_back(":"+QString::number(ntohs(client.sin_port)));
    clientName.push_front("🟢");
    for(int i =0; i<comboBoxText.size();i++){
        if(clientName==ui->comboBox->itemText(i)){
            clientName.remove(0,2);
            clientName.push_front("❌ ");
            ui->comboBox->setItemText(i,clientName);
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    InitTraceWindow();
}


void MainWindow::on_pushButton_2_clicked()
{
    //Чтение из файла
    QString fileName = QFileDialog::getOpenFileName(this);
    fileReader = new FileReader();
    traceWindow = new TraceWindow();

    fileReader->setTraceWindow(traceWindow);
    fileReader->setFileName(fileName);
    fileReader->start();


}

void MainWindow::InitTraceWindow()
{
    //Говнокод, пофиксить надо потом
    QString tempComboBoxText = ui->comboBox->currentText();
    if(tempComboBoxText.startsWith("❌ ")){
        QMessageBox mbx;
        mbx.setText("This connection was terminated");
        mbx.exec();
        return;
    }
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
            traceWindows.append(traceWindow);
            traceWindow->setClientName(comboBoxText);
            traceWindow->show();
            traceWindow->setStyle(styleSheet);

        }
    }
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

