#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "includes/chunkhandler.h"
#include "includes/launcher.h"
#include "includes/packethandler.h"

//Главное окно, отсюда создаются новые потоки traceWindow и ведется управление соединениями

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    setWindowIcon(QIcon("logo.png"));
    ui->setupUi(this);

    connect(ui->connectionsTable,&QTableWidget::customContextMenuRequested,this,&MainWindow::showContextMenu);

    ui->connectionsTable->setFocusPolicy(Qt::NoFocus);
    ui->connectionsTable->setSelectionBehavior(QTableWidget::SelectRows);
    ui->connectionsTable->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->connectionsTable->setColumnCount(2);
    ui->connectionsTable->setColumnWidth(0,300);
    ui->connectionsTable->setColumnWidth(1,50);

    ui->connectionsTable->horizontalHeader()->hide();
    ui->connectionsTable->verticalHeader()->hide();
    ui->connectionsTable->horizontalHeader()->setStretchLastSection(true);
    this->setWindowIcon(QIcon(":/baical_icon.png"));
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QTableWidgetItem *item = ui->connectionsTable->itemAt(pos);
    if(item!=NULL){
        QString pathToFile;
        //Проверка является ли открываемый объект файлом
        if(launcher->clientsList->at(item->row()).connectionThread==NULL){
            pathToFile = traceWindows.at(item->row())->getClientName().port;
        }
        else{
            pathToFile = launcher->clientsList->at(item->row()).connectionThread->chunkHandler.getBackupFileName();
        }
        pathToFile = pathToFile.replace("/","\\");
        QString fileName = "/select,"+pathToFile;
        QProcess::startDetached("C:\\Windows\\explorer.exe", {fileName});
    }
}

void MainWindow::setLauncher(Launcher* newLauncher)
{
    launcher = newLauncher;
    connect(this,&MainWindow::sendClientToDelete,launcher,&Launcher::deleteClient);
}

MainWindow::~MainWindow()
{
    std::cout<<"------"<<"Closing MainWindow thread"<<"------"<<std::endl;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
}

void MainWindow::getNewConnection(sockaddr_in newConnection, PacketHandler* packetHandler)
{
    std::cout<<"New connection from:"<< ntohs(newConnection.sin_port)<<std::endl;
    ConnectionName connectionName = {inet_ntoa(newConnection.sin_addr),QString::number(ntohs(newConnection.sin_port))};
    ui->connectionsTable->insertRow(ui->connectionsTable->rowCount());

    initTraceWindow(connectionName);

    QTableWidgetItem* connectionWidgetItem = new QTableWidgetItem(connectionName.ip+":"+connectionName.port);

    QPushButton* closeButton = new QPushButton("Clear");

    if(currentTheme==DARK){
        closeButton->setStyleSheet("QPushButton:disabled {background-color:#606060; color:#A0A0A0}");
    }

    closeButton->setDisabled(true);
    closeButton->setToolTip(QString::number(ui->connectionsTable->rowCount()-1));

    connect(closeButton, &QPushButton::clicked, this, &MainWindow::onCloseConnectionClicked);

    connectionWidgetItem->setIcon(QIcon(":/green-dot.png"));
    connectionWidgetItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);


    ui->connectionsTable->setItem(ui->connectionsTable->rowCount()-1,0,connectionWidgetItem);
    ui->connectionsTable->setCellWidget(ui->connectionsTable->rowCount()-1,1,closeButton);



    ui->statusbar->showMessage("New connection from: "+ connectionName.ip+":"+connectionName.port);

    packetHandler->start();
}

void MainWindow::changeClientStatus(sockaddr_in client, tUINT32 status)
{
    QString clientName = inet_ntoa(client.sin_addr);
    clientName.push_back(":"+QString::number(ntohs(client.sin_port)));

    for(int i =0;i<ui->connectionsTable->rowCount();i++){
        if(ui->connectionsTable->item(i,0)->text()==clientName){
            if(status==OFFLINE){
                ui->connectionsTable->item(i,0)->setIcon(QIcon(":/red-dot.png"));
                traceWindows.at(i)->setConnectionStatus(0);
                ui->connectionsTable->cellWidget(i,1)->setDisabled(false);
            } else if(status==UNKNOWN_CONNECTION_STATUS){
                ui->connectionsTable->item(i,0)->setIcon(QIcon(":/yellow-dot.png"));
                traceWindows.at(i)->setConnectionStatus(1);
            } else if(status==ONLINE){
                ui->connectionsTable->item(i,0)->setIcon(QIcon(":/green-dot.png"));
                traceWindows.at(i)->setConnectionStatus(2);
            }
        }
    }
}

void MainWindow::onCloseConnectionClicked()
{
    QPushButton* button = qobject_cast<QPushButton*> (sender());
    tUINT32 row = button->toolTip().toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Delete window?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        TraceWindow* traceWnd= traceWindows.at(row);
        if(traceWnd->getConnectionStatus()!=OFFLINE && traceWnd->getConnectionStatus()!=FILE_CONNECTION){
            ui->statusbar->showMessage("Can't close active connection!");
            return;
        }

        ui->statusbar->showMessage("Deleted connection: "+ traceWnd->getClientName().ip+":"+traceWnd->getClientName().port);

        emit sendClientToDelete(row);

        traceWindows.removeAt(row);
        ui->connectionsTable->removeRow(row);

        //QWindow->deleteLater почему-то иногда вызывает полный статтер ГУИ
        //Для избежания этого просто чистим дату внутри окна, как пишут умные люди интернета
        //QT сам чистит окна когда надо
        if(traceWnd->getFileReader()!=NULL && !traceWnd->getFileReader()->isFinished()){
            traceWnd->getFileReader()->requestInterruption();
        }
        else{
            //            traceWnd->clearData();
            traceWnd->deleteLater();
        }

    }
    //    QApplication::processEvents();
    std::cout<<"deleted"<<std::endl;

    for(int i =0;i<ui->connectionsTable->rowCount();i++){
        QPushButton* t_button = (QPushButton*) ui->connectionsTable->cellWidget(i,1);
        t_button->setToolTip(QString::number(i));
    }
}


TraceWindow* MainWindow::initTraceWindow(ConnectionName connectionName)
{
    traceWindow = new TraceWindow(connectionName,config,this);
    traceWindow->setConnectionStatus(ONLINE);
    traceWindows.append(traceWindow);
    tUINT32 index = traceWindows.size()-1;
    launcher->clientsList->at(index).connectionThread->chunkHandler.setTraceWindow(traceWindow);

    if(ui->actionAuto_Open->isChecked()){
        traceWindow->show();
    }

    traceWindow->changeTheme(currentTheme);

    return traceWindow;
}

void MainWindow::on_actionHigh_Contrast_Black_triggered()
{
    ui->connectionsTable->setStyleSheet("color: white;"
                                        "background-color: rgb(28,28,28);"
                                        "selection-background-color: rgb(63, 100, 204);"
                                        "selection-color: rgb(255, 255, 255); "
                                        "alternate-background-color: rgb(255, 255, 255);");

    for(int i = 0;i<ui->connectionsTable->rowCount();i++){
        ui->connectionsTable->item(i,0)->setBackgroundColor(QColor(36,36,36));
        ui->connectionsTable->cellWidget(i,1)->setStyleSheet("QPushButton:disabled {background-color:#121212; color:#484848}");
    }

    ui->menubar->setStyleSheet("QMenuBar::item:selected { background: #2b2b2b; } "
                               "QMenuBar::item:pressed {  background: #2b2b2b; }"
                               );

    this->setStyleSheet("color: white; background-color: rgb(0,0,0); "
                        "selection-background-color: rgb(63, 100, 204);");
    for(int i = 0;i<traceWindows.size();i++){
        traceWindows.at(i)->changeTheme(DARK);
    }
    currentTheme = DARK;
}

void MainWindow::on_actionLike_in_QT_triggered()
{
    styleSheet = "color: white; background-color: rgb(42,43,44);";
    this->setStyleSheet(styleSheet);
    for(int i = 0;i<traceWindows.length();i++){
        //        traceWindows.at(i)->setStyle(styleSheet);
    }
}

void MainWindow::on_actionWhite_triggered()
{
    styleSheet = "";
    ui->connectionsTable->setStyleSheet("");
    ui->menubar->setStyleSheet("");

    for(int i = 0;i<ui->connectionsTable->rowCount();i++){
        ui->connectionsTable->item(i,0)->setBackgroundColor(Qt::white);
        ui->connectionsTable->cellWidget(i,1)->setStyleSheet("");
    }

    this->setStyleSheet(styleSheet);
    for(int i = 0;i<traceWindows.size();i++){
        traceWindows.at(i)->changeTheme(DEFAULT);
    }
    currentTheme = DEFAULT;
}

void MainWindow::on_connectionsTable_cellDoubleClicked(int row, int column)
{
    std::cout<<row<<std::endl;
    if(column==0){
        //Open
        if(traceWindows.at(row)->isVisible()){
            traceWindows.at(row)->raise();
            return;
        }
        traceWindows.at(row)->show();
    }
}

void MainWindow::on_actionOpen_File_triggered()
{
    //Чтение из файла
    QString fileName = QFileDialog::getOpenFileName(this);
    if(fileName==""){
        return;
    }

    ui->connectionsTable->insertRow(ui->connectionsTable->rowCount());
    QTableWidgetItem* connectionWidgetItem = new QTableWidgetItem(fileName);

    connectionWidgetItem->setIcon(QIcon(":/baicalFile.png"));
    connectionWidgetItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    QPushButton* closeButton = new QPushButton("Clear");
    if(currentTheme==DARK){
        closeButton->setStyleSheet("QPushButton:disabled {background-color:#606060; color:#A0A0A0}");
    }
    closeButton->setToolTip(QString::number(ui->connectionsTable->rowCount()-1));

    connect(closeButton, &QPushButton::clicked, this, &MainWindow::onCloseConnectionClicked);

    ui->connectionsTable->setItem(ui->connectionsTable->rowCount()-1,0,connectionWidgetItem);
    ui->connectionsTable->setCellWidget(ui->connectionsTable->rowCount()-1,1,closeButton);

    ui->statusbar->showMessage("File opened: "+ fileName);

    launcher->clientsList->append({NULL,NULL});
    traceWindow = new TraceWindow({"File ",fileName},config,this);
    fileReader = new FileReader(fileName,traceWindow);
    traceWindow->setFileReader(fileReader);
    traceWindow->setTraceAsObject(fileReader->chunkHandler.getTraceHandler());
    traceWindow->setConnectionStatus(FILE_CONNECTION);
    traceWindows.append(traceWindow);

    if(ui->actionAuto_Open->isChecked()){
        traceWindow->show();
    }

    traceWindow->changeTheme(currentTheme);

    fileReader->start();
    ui->statusbar->showMessage("New file opened: "+ fileName);
}

