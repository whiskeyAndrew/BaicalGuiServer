#include "tracewindowsettings.h"
#include "mainwindow.h"
#include "ui_tracewindowsettings.h"
#include "tracewindow.h"
#include <QObject>

TraceWindowSettings::TraceWindowSettings(TraceWindow* newTraceWindow, ConnectionName* clientName):ui(new Ui::TraceWindowSettings)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/baical_icon.png"));
    traceWindow = newTraceWindow;
    enumParser = new EnumParser();

    //0 элемент в enumParser->enums - элемент, который будет отвечать за разделение по символам элементов
    QMap<tUINT32,enumFromFile> e;
    //филлер
    e.insert(0,{"0","0"});
    //    enumParser->enums.append({"1234567 -> 1 234 567",e});

    connectionName =*clientName;

    initWindow();
}

void TraceWindowSettings::initWindow()
{
    setWindowTitle(connectionName.ip+":"+connectionName.port +" settings");
    ui->traceIDforEnums->addItem("");
    traceIDforEnumsListForFastChanging.append(0);

    ui->rawTracesTable->insertColumn(0);
    ui->rawTracesTable->setColumnWidth(0, ui->rawTracesTable->width()/10);
    ui->rawTracesTable->insertColumn(1);
    ui->rawTracesTable->horizontalHeader()->setStretchLastSection(true);
    autoTracesCount = ui->autoRowsCounter;

    connect(ui->listWidget,&QListWidget::itemChanged,this,&TraceWindowSettings::uniqueTracesItemChanged);
    connect(ui->modulesList,&QListWidget::itemChanged,this,&TraceWindowSettings::modulesItemChanged);

    colorDialog = new QColorDialog();
    colorDialog->setOption(QColorDialog::ShowAlphaChannel);

    if(connectionName.ip.startsWith("File")){
        QString configSettingsNameGenerated = connectionName.port.mid(connectionName.port.lastIndexOf("/"));
        configSettingsName = configSettingsNameGenerated;
    }else{
        configSettingsName = connectionName.ip;
    }
    config = new ConfigHandler(configSettingsName);

    initTraceLevels();
    initColors();
    initWindowsSize();
    loadTracesToShowByIdFromConfig();
    loadModulesToShowFromConfig();
    loadColumnsFromConfig();

    wheelScrollStep = config->loadWheelScrollStep(configSettingsName);
    ui->wheelSpin->setValue(wheelScrollStep);

    QString enumsFile = config->loadEnumsList(configSettingsName);
    if(enumsFile!=""){
        if(loadEnumsFromFile(enumsFile)){
            loadEnumsFromConfig();
        }
    } else{
        loadEnumsFromConfig();
    }

    ui->rowsOnScreen->setValidator(new QIntValidator(0, INT_MAX, this));
    //    ui->wheelStepLineEdit->setValidator(new QIntValidator(0, INT_MAX, this));
    loadConfigFileAsText();

    //NULL module init
    QListWidgetItem* listItem = new QListWidgetItem();
    listItem->setText("NO MODULE");
    listItem->setData(Qt::ToolTipRole,0);
    listItem->setCheckState(Qt::Checked);
    ui->modulesList->addItem(listItem);

    needToShowModules.insert(0,Qt::Checked);
}


TraceWindowSettings::~TraceWindowSettings()
{
    std::cout<<"------Deleting TraceWindowSettings------"<<std::endl;
    delete ui;
}

void TraceWindowSettings::uniqueTracesItemChanged(QListWidgetItem* item)
{
    tUINT32 wID = item->data(Qt::ToolTipRole).toInt();
    tUINT32 state = item->checkState();
    needToShowTraceByID.insert(wID,state);
    traceWindow->recountNubmerOfTracesToShow();
    traceWindow->reloadTracesInsideWindow();

}

void TraceWindowSettings::modulesItemChanged(QListWidgetItem* item)
{
    tUINT16 id = item->data(Qt::ToolTipRole).toInt();
    tUINT32 state = item->checkState();
    needToShowModules.insert(id,state);
    traceWindow->recountNubmerOfTracesToShow();
    traceWindow->reloadTracesInsideWindow();
}

void TraceWindowSettings::appendUniqueTracesList(QString text, tUINT32 wID)
{
    QListWidgetItem* listItem = new QListWidgetItem();
    listItem->setText(QString::number(wID) + " " +text);
    listItem->setData(Qt::ToolTipRole,wID);

    if(config->getTracesToShowByIdFromConfig().contains(wID)){
        needToShowTraceByID.insert(wID,config->getTracesToShowByIdFromConfig().value(wID));
        listItem->setCheckState((Qt::CheckState)config->getTracesToShowByIdFromConfig().value(wID));
    }
    else{
        needToShowTraceByID.insert(wID,Qt::Checked);
        listItem->setCheckState(Qt::Checked);
    }

    ui->listWidget->addItem(listItem);

    ui->traceIDforEnums->addItem(QString::number(wID));
    traceIDforEnumsListForFastChanging.append(wID);
}

void TraceWindowSettings::appendModules(sP7Trace_Module module)
{
    QString moduleName(module.pName);
    QListWidgetItem* listItem = new QListWidgetItem();
    listItem->setText(moduleName);
    listItem->setData(Qt::ToolTipRole,module.wModuleId);

    if(config->getNeedToShowModules().contains(module.wModuleId)){
        needToShowModules.insert(module.wModuleId,config->getNeedToShowModules().value(module.wModuleId));
        listItem->setCheckState((Qt::CheckState)config->getNeedToShowModules().value(module.wModuleId));
    }else{
        needToShowModules.insert(module.wModuleId,Qt::Checked);
        listItem->setCheckState(Qt::Checked);
    }

    ui->modulesList->addItem(listItem);
}

void TraceWindowSettings::disableElement(tUINT32 wID)
{
    needToShowTraceByID.insert(wID,Qt::Unchecked);
    for(int i =0;i<ui->listWidget->count();i++){
        if(ui->listWidget->item(i)->data(Qt::ToolTipRole)==wID){
            ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
            break;
        }
    }
}

void TraceWindowSettings::on_traceColorButton_clicked()
{
    color = colorDialog->getColor();
    if(!color.isValid()){
        return;
    }
    ui->traceColorButton->setStyleSheet("background-color: rgb("+QString::number(color.red())+", "
                                        +QString::number(color.green())+", "
                                        +QString::number(color.blue())+");"+
                                        "selection-color: rgb("+QString::number(color.red())+", "
                                        +QString::number(color.green())+", "
                                        +QString::number(color.blue())+");");
    traceWindow->setTraceColor(color);
    config->traceColor=color;
}

void TraceWindowSettings::on_debugColorButton_clicked()
{
    color = colorDialog->getColor();
    if(!color.isValid()){
        return;
    }
    ui->debugColorButton->setStyleSheet("background-color: rgb("+QString::number(color.red())+", "
                                        +QString::number(color.green())+", "
                                        +QString::number(color.blue())+");"+
                                        "selection-color: rgb("+QString::number(color.red())+", "
                                        +QString::number(color.green())+", "
                                        +QString::number(color.blue())+");");
    traceWindow->setDebugColor(color);
    config->debugColor=color;
}

void TraceWindowSettings::on_infoColorButton_clicked()
{
    color = colorDialog->getColor();
    if(!color.isValid()){
        return;
    }
    ui->infoColorButton->setStyleSheet("background-color: rgb("+QString::number(color.red())+", "
                                       +QString::number(color.green())
                                       +", "+QString::number(color.blue())+");"+
                                       "selection-color: rgb("+QString::number(color.red())+", "
                                       +QString::number(color.green())+", "
                                       +QString::number(color.blue())+");");
    traceWindow->setInfoColor(color);
    config->infoColor=color;
}

void TraceWindowSettings::on_warningColorButton_clicked()
{
    color = colorDialog->getColor();
    if(!color.isValid()){
        return;
    }
    ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(color.red())+", "
                                          +QString::number(color.green())+", "
                                          +QString::number(color.blue())+");"+
                                          "selection-color: rgb("+QString::number(color.red())+", "
                                          +QString::number(color.green())+", "
                                          +QString::number(color.blue())+");");
    traceWindow->setWarningColor(color);
    config->warningColor=color;
}

void TraceWindowSettings::on_errorColorButton_clicked()
{
    color = colorDialog->getColor();
    if(!color.isValid()){
        return;
    }
    ui->errorColorButton->setStyleSheet("background-color: rgb("+QString::number(color.red())+", "
                                        +QString::number(color.green())+", "
                                        +QString::number(color.blue())+");"+
                                        "selection-color: rgb("+QString::number(color.red())+", "
                                        +QString::number(color.green())+", "
                                        +QString::number(color.blue())+");");
    traceWindow->setErrorColor(color);
    config->errorColor=color;
}

void TraceWindowSettings::on_criticalColorButton_clicked()
{
    color = colorDialog->getColor();
    if(!color.isValid()){
        return;
    }

    ui->criticalColorButton->setStyleSheet("background-color: rgb("+QString::number(color.red())+", "
                                           +QString::number(color.green())+", "
                                           +QString::number(color.blue())+");"+
                                           "selection-color: rgb("+QString::number(color.red())+", "
                                           +QString::number(color.green())+", "
                                           +QString::number(color.blue())+");");
    traceWindow->setCriticalColor(color);
    config->criticalColor=traceWindow->getCriticalColor();
}

void TraceWindowSettings::on_clearTrace_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Trace", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setTraceColor(traceWindow->getEmptyColor());
        config->traceColor = traceWindow->getEmptyColor();
        ui->traceColorButton->setStyleSheet("");
    }

}

void TraceWindowSettings::on_clearDebug_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Debug", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setDebugColor(traceWindow->getEmptyColor());
        config->debugColor = traceWindow->getEmptyColor();
        ui->debugColorButton->setStyleSheet("");
    }
}

void TraceWindowSettings::on_clearInfo_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Info", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setInfoColor(traceWindow->getEmptyColor());
        config->infoColor = traceWindow->getEmptyColor();
        ui->infoColorButton->setStyleSheet("");
    }
}

void TraceWindowSettings::on_clearWarning_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Warning", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setWarningColor(traceWindow->getEmptyColor());
        config->warningColor = traceWindow->getEmptyColor();
        ui->warningColorButton->setStyleSheet("");
    }
}

void TraceWindowSettings::on_clearError_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Error", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setErrorColor(traceWindow->getEmptyColor());
        config->errorColor = traceWindow->getEmptyColor();
        ui->errorColorButton->setStyleSheet("");
    }
}

void TraceWindowSettings::on_clearCritical_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Critical", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setCriticalColor(traceWindow->getEmptyColor());
        config->criticalColor = traceWindow->getEmptyColor();
        ui->criticalColorButton->setStyleSheet("");
    }
}

Qt::CheckState TraceWindowSettings::isTraceColumnNeedToShow()
{
    return ui->traceCheckbox->checkState();
}

Qt::CheckState TraceWindowSettings::isTimeColumnNeedToShow()
{
    return ui->timeCheckbox->checkState();
}

Qt::CheckState TraceWindowSettings::isSequenceColumnNeedToShow()
{
    return ui->sequenceCheckbox->checkState();
}

void TraceWindowSettings::initTraceLevels()
{
    config->loadTraceLevelsToShow();
    ui->traceLevelCheckBox->setCheckState(config->traceLevel);
    ui->debugCheckBox->setCheckState(config->debugLevel);
    ui->infoCheckBox->setCheckState(config->infoLevel);
    ui->warningCheckBox->setCheckState(config->warningLevel);
    ui->errorCheckBox->setCheckState(config->errorLevel);
    ui->criticalCheckBox->setCheckState(config->criticalLevel);

}

void TraceWindowSettings::initColors()
{
    config->loadColors();
    if(config->traceColor.isValid()){
        ui->traceColorButton->setStyleSheet("background-color: rgb("+QString::number(config->traceColor.red())+", "
                                            +QString::number(config->traceColor.green())+", "
                                            +QString::number(config->traceColor.blue())+");"+
                                            "selection-color: rgb("+QString::number(config->traceColor.red())+", "
                                            +QString::number(config->traceColor.green())+", "
                                            +QString::number(config->traceColor.blue())+");");
        traceWindow->setTraceColor(config->traceColor);
    } else{
        ui->traceColorButton->setStyleSheet("");
    }

    if(config->debugColor.isValid()){
        ui->debugColorButton->setStyleSheet("background-color: rgb("+QString::number(config->debugColor.red())+", "
                                            +QString::number(config->debugColor.green())+", "
                                            +QString::number(config->debugColor.blue())+");"+

                                            "selection-color: rgb("+QString::number(config->debugColor.red())+", "
                                            +QString::number(config->debugColor.green())+", "
                                            +QString::number(config->debugColor.blue())+");");
        traceWindow->setDebugColor(config->debugColor);
    } else{
        ui->debugColorButton->setStyleSheet("");
    }

    if(config->infoColor.isValid()){
        ui->infoColorButton->setStyleSheet("background-color: rgb("+QString::number(traceWindow->getInfoColor().red())+", "
                                           +QString::number(config->infoColor.green())+", "
                                           +QString::number(config->infoColor.blue())+");"+
                                           "selection-color: rgb("+QString::number(config->infoColor.red())+", "
                                           +QString::number(config->infoColor.green())+", "
                                           +QString::number(config->infoColor.blue())+");");
        traceWindow->setInfoColor(config->infoColor);
    } else{
        ui->infoColorButton->setStyleSheet("");
    }

    if(config->warningColor.isValid()){
        ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(config->warningColor.red())+", "
                                              +QString::number(config->warningColor.green())+", "
                                              +QString::number(config->warningColor.blue())+");"+
                                              "selection-color: rgb("+QString::number(config->warningColor.red())+", "
                                              +QString::number(config->warningColor.green())+", "
                                              +QString::number(config->warningColor.blue())+");");
        traceWindow->setWarningColor(config->warningColor);
    } else{
        ui->warningColorButton->setStyleSheet("");
    }

    if(config->errorColor.isValid()){
        ui->errorColorButton->setStyleSheet("background-color: rgb("+QString::number(config->errorColor.red())+", "
                                            +QString::number(config->errorColor.green())+", "
                                            +QString::number(config->errorColor.blue())+");"+
                                            "selection-color: rgb("+QString::number(config->errorColor.red())+", "
                                            +QString::number(config->errorColor.green())+", "
                                            +QString::number(config->errorColor.blue())+");");
        traceWindow->setErrorColor(config->errorColor);
    } else{
        ui->errorColorButton->setStyleSheet("");
    }

    if(config->criticalColor.isValid()){
        ui->criticalColorButton->setStyleSheet("background-color: rgb("+QString::number(config->criticalColor.red())+", "
                                               +QString::number(config->criticalColor.green())+", "
                                               +QString::number(config->criticalColor.blue())+");"+
                                               "selection-color: rgb("+QString::number(config->criticalColor.red())+", "
                                               +QString::number(config->criticalColor.green())+", "
                                               +QString::number(config->criticalColor.blue())+");");
        traceWindow->setCriticalColor(config->criticalColor);
    } else{
        ui->criticalColorButton->setStyleSheet("");
    }

}

void TraceWindowSettings::on_traceLevelCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_TRACE,arg1);
    config->traceLevel = static_cast<Qt::CheckState>(arg1);
    traceWindow->recountNubmerOfTracesToShow();
}


void TraceWindowSettings::on_debugCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_DEBUG,arg1);
    config->debugLevel = static_cast<Qt::CheckState>(arg1);
    traceWindow->recountNubmerOfTracesToShow();
}


void TraceWindowSettings::on_infoCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_INFO,arg1);
    config->infoLevel = static_cast<Qt::CheckState>(arg1);
    traceWindow->recountNubmerOfTracesToShow();
}


void TraceWindowSettings::on_warningCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_WARNING,arg1);
    config->warningLevel = static_cast<Qt::CheckState>(arg1);
    traceWindow->recountNubmerOfTracesToShow();
}


void TraceWindowSettings::on_errorCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_ERROR,arg1);
    config->errorLevel = static_cast<Qt::CheckState>(arg1);
    traceWindow->recountNubmerOfTracesToShow();
}


void TraceWindowSettings::on_criticalCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_CRITICAL,arg1);
    config->criticalLevel = static_cast<Qt::CheckState>(arg1);
    traceWindow->recountNubmerOfTracesToShow();
}


void TraceWindowSettings::on_sequenceCheckbox_stateChanged(int arg1)
{

    //Неоптимизированно, т.к. быстрый перепил старого функционала, потом поправить
    QMap<QString, Qt::CheckState> typesToShow;
    typesToShow.insert("sequence",ui->sequenceCheckbox->checkState());
    config->saveColumnsToShow(configSettingsName,typesToShow);
    traceWindow->reloadTracesInsideWindow();
}

void TraceWindowSettings::on_traceCheckbox_stateChanged(int arg1)
{

    //Неоптимизированно, т.к. быстрый перепил старого функционала, потом поправить
    QMap<QString, Qt::CheckState> typesToShow;
    typesToShow.insert("trace",ui->traceCheckbox->checkState());
    config->saveColumnsToShow(configSettingsName,typesToShow);
    traceWindow->reloadTracesInsideWindow();
}

void TraceWindowSettings::on_timeCheckbox_stateChanged(int arg1)
{

    //Неоптимизированно, т.к. быстрый перепил старого функционала, потом поправить
    QMap<QString, Qt::CheckState> typesToShow;
    typesToShow.insert("time",ui->timeCheckbox->checkState());
    config->saveColumnsToShow(configSettingsName,typesToShow);
    traceWindow->reloadTracesInsideWindow();
}

void TraceWindowSettings::on_checkAllUniqueTraces_clicked()
{
    ui->listWidget->blockSignals(true);
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setCheckState(Qt::CheckState::Checked);
        tUINT32 wID = ui->listWidget->item(i)->data(Qt::ToolTipRole).toInt();
        tUINT32 state = ui->listWidget->item(i)->checkState();
        needToShowTraceByID.insert(wID,state);
    }
    ui->listWidget->blockSignals(false);
    traceWindow->recountNubmerOfTracesToShow();
}

void TraceWindowSettings::on_tabWidget_tabBarClicked(int index)
{
    //Менять значение если добавляешь дополнительные вкладки в traceWindowSettings
    if(index==4){
        loadConfigFileAsText();
    }
}

void TraceWindowSettings::loadConfigFileAsText(){
    QFile file(config->getConfigFileName());

    if (!file.open(QIODevice::ReadOnly |  QIODevice::Text)){
        return;
    }

    ui->configText->setPlainText("");
    QTextStream in(&file);
    while (!in.atEnd()) {
        ui->configText->appendPlainText(in.readLine());
    }
    file.close();
    traceWindow->setActionStatusText("Config was loaded!");
}

void TraceWindowSettings::initWindowsSize()
{
    config->loadWindowsSize();
    if(config->traceWindow_x!=0 || config->traceWindow_y!=0){
        traceWindow->resize(config->traceWindow_x,config->traceWindow_y);
    }

    if(config->traceSettingsWindow_x!=0 ||config->traceSettingsWindow_y!=0){
        this->resize(config->traceSettingsWindow_x,config->traceSettingsWindow_y);
    }
}

void TraceWindowSettings::on_loadButton_clicked()
{
    QString fileName= QFileDialog::getOpenFileName(this, "Load",config->getConfigFileName(),tr("Config files(*.ini)"));

    if(fileName==""){
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly |  QIODevice::Text)){
        return;
    }

    ui->configText->setPlainText("");
    QTextStream in(&file);
    while (!in.atEnd()) {
        ui->configText->appendPlainText(in.readLine());
    }
    file.close();
    traceWindow->setActionStatusText("Config was loaded!");
}

void TraceWindowSettings::on_saveButton_clicked()
{
    QFile file(config->getConfigFileName());

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
    QString text = ui->configText->toPlainText();
    std::cout<<text.toStdString()<<std::endl;
    out<< text;
    file.close();
    traceWindow->setActionStatusText("Config was saved!");
}

void TraceWindowSettings::on_saveAsButton_clicked()
{
    QString fileName= QFileDialog::getSaveFileName(this, "Save As",config->getConfigFileName(),tr("Config files(*.ini"));

    if(fileName==""){
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
    QString text = ui->configText->toPlainText();
    std::cout<<text.toStdString()<<std::endl;
    out<< text;
    file.close();
    traceWindow->setActionStatusText("Config was saved!");
}

void TraceWindowSettings::on_LoadDataFromConfig_clicked()
{
    initColors();
    initTraceLevels();
}


void TraceWindowSettings::on_uncheckAllUniqueTraces_clicked()
{
    ui->listWidget->blockSignals(true);
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setCheckState(Qt::CheckState::Unchecked);
        tUINT32 wID = ui->listWidget->item(i)->data(Qt::ToolTipRole).toInt();
        tUINT32 state = ui->listWidget->item(i)->checkState();
        needToShowTraceByID.insert(wID,state);
    }
    ui->listWidget->blockSignals(false);
    traceWindow->recountNubmerOfTracesToShow();
}

void TraceWindowSettings::on_saveAllTraceCheckboxes_clicked()
{
    config->saveTraceLevelsToShow();
    traceWindow->setActionStatusText("Traces checkboxes was saved!");
}

void TraceWindowSettings::on_saveAllColors_clicked()
{
    config->saveColors();
    traceWindow->setActionStatusText("Traces colors was saved!");
}

void TraceWindowSettings::on_autoRowsCounter_stateChanged(int arg1)
{
    if(arg1==Qt::CheckState::Unchecked){
        ui->rowsOnScreen->setEnabled(true);
        if(ui->rowsOnScreen->text()!=""){
            traceWindow->recountNumberOfRowsToShow();
            return;
        }
    } else{
        ui->rowsOnScreen->setEnabled(false);
        traceWindow->recountNumberOfRowsToShow();
    }
}

void TraceWindowSettings::resizeEvent(QResizeEvent* e){
    ui->traceSettingsWindowSizeLabel->setText(QString::number(this->size().width())+"px width / " +QString::number(this->size().height())+"px height");
}

void TraceWindowSettings::setTraceWindowSizeText(){
    ui->traceWindowSizeLabel->setText(QString::number(traceWindow->size().width())+"px width / " +QString::number(traceWindow->size().height())+"px height");
}

QString TraceWindowSettings::getRowsOnScreen()
{
    return ui->rowsOnScreen->text();
}

QCheckBox* TraceWindowSettings::getAutoTracesCount() const
{
    return autoTracesCount;
}

void TraceWindowSettings::on_rowsOnScreen_editingFinished()
{
    traceWindow->recountNumberOfRowsToShow();
}


void TraceWindowSettings::on_saveWindowsProperties_clicked()
{
    config->saveWindowsSize(traceWindow->size().width(),traceWindow->size().height(),this->size().width(),traceWindow->size().height());
}


void TraceWindowSettings::on_loadEnumsFromTXT_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Save As",nullptr,tr("Text files(*.txt)"));
    loadEnumsFromFile(fileName);
}

tBOOL TraceWindowSettings::loadEnumsFromFile(QString fileName){
    if(!enumParser->readEnumsFromFile(fileName)){
        ui->enumsStatus->setText("Can't load enum from config");
        return false;
    }
    enumsIdList.clear();


    ui->enumsList->clear();

    //Начинаем итерацию с 101, так как 0-100 зарезервированы для внедрения преобразований кодом
    for(int i =101;i<enumParser->enums.size();i++){
        QListWidgetItem* item = new QListWidgetItem(enumParser->enums.at(i).name);
        item->setData(Qt::ToolTipRole,i);
        ui->enumsList->addItem(item);
        enumsIdList.append(i);
    }
    config->saveEnumsList(configSettingsName,fileName);
    ui->enumsStatus->setText("Loaded enums from " + fileName +": " +QString::number(enumParser->enums.size()));
    return true;
}

void TraceWindowSettings::on_enumsList_itemClicked(QListWidgetItem* item)
{
    tUINT32 rowId = item->data(Qt::ToolTipRole).toInt();
    likeEnum _enum = enumParser->enums.at(rowId);
    int enumId = 0;
    ui->enumsElements->clear();

    for(tUINT32 key:_enum.enums.keys()){
        QListWidgetItem* item = new QListWidgetItem(_enum.enums.value(key).name+" "+QString::number(key) +" " +_enum.enums.value(key).comment);
        item->setData(Qt::ToolTipRole,++enumId);
        ui->enumsElements->addItem(item);
    }
}


void TraceWindowSettings::on_applyEnumToTraceById_clicked(){
    if(ui->traceIDforEnums->currentText()==""){
        return;
    }

    if(ui->rawTracesTable->rowCount()==0){
        return;
    }

    QList<ArgsThatNeedToBeChangedByEnum> args;
    for(int i =0;i<ui->rawTracesTable->rowCount();i++){
        tUINT32 argId = ui->rawTracesTable->item(i,0)->text().toInt();
        QComboBox* comboBox = qobject_cast<QComboBox*>(ui->rawTracesTable->cellWidget(i,1));
        tUINT32 enumId = comboBox->itemData(comboBox->currentIndex(),Qt::ToolTipRole).toInt();
        args.append({argId,enumId,ui->rawTracesTable->item(i,0)->checkState()});
    }

    traceWindow->appendArgsThatNeedToBeChangedByEnum(ui->traceIDforEnums->currentText().toInt(),args);
    ui->enumsStatus->setText("Applied");
}

void TraceWindowSettings::changeTraceIDforEnumsValue(tUINT32 wID)
{
    ui->traceIDforEnums->setCurrentIndex( traceIDforEnumsListForFastChanging.indexOf(wID));
}

EnumParser* TraceWindowSettings::getEnumParser() const
{
    return enumParser;
}


void TraceWindowSettings::on_traceIDforEnums_currentIndexChanged(int index)
{
    reloadListOfArgsAndEnums();
}


void TraceWindowSettings::on_saveAllSettings_clicked()
{
    config->saveColors();
    traceWindow->setActionStatusText("Traces colors was saved!");
    config->saveTraceLevelsToShow();
    traceWindow->setActionStatusText("Traces checkboxes was saved!");
    config->saveWindowsSize(traceWindow->size().width(),traceWindow->size().height(),this->size().width(),traceWindow->size().height());
}


void TraceWindowSettings::on_saveEnumsToConfig_clicked()
{
    ui->enumsStatus->setText("Saved enums: " + QString::number(config->saveEnums(traceWindow->getArgsThatNeedToBeChangedByEnum(),configSettingsName)));
}


void TraceWindowSettings::on_clearEnums_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Enums", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    ui->enumsStatus->setText("Cleared enums: "+ QString::number(traceWindow->getArgsThatNeedToBeChangedByEnum().size()));
    for(int i =0;i<ui->rawTracesTable->rowCount();i++){
        QComboBox* comboBox = qobject_cast<QComboBox*>(ui->rawTracesTable->cellWidget(i,1));
        comboBox->setCurrentIndex(0);
    }
    traceWindow->clearArgsThatNeedToBeChangedByEnumm();
    reloadListOfArgsAndEnums();
}

void TraceWindowSettings::reloadListOfArgsAndEnums(){
    ui->rawTracesTable->setRowCount(0);

    for(int i =0;i<comboBoxesToDelete.size();i++){
        QComboBox* comboBox = comboBoxesToDelete.at(i);
        delete comboBox;
    }
    comboBoxesToDelete.clear();

    if(ui->traceIDforEnums->currentText()==""){
        ui->uniqueTraceLabel->setText("");
        return;
    }

    tUINT32 wID = ui->traceIDforEnums->currentText().toInt();
    Trace* traceHandler = traceWindow->traceThread;
    ui->uniqueTraceLabel->setText(traceWindow->traceThread->getUniqueTraces().value(wID).traceLineForEnumWindow);

    //Если у нас уже есть изменения по wID, подгружаем их
    if(traceWindow->getArgsThatNeedToBeChangedByEnum().contains(wID)){
        for(int i =0;i<traceHandler->getUniqueTraces().value(wID).argsID.size();i++){
            int countNumber = ui->rawTracesTable->rowCount();
            QComboBox* comboBox = new QComboBox();
            comboBox->addItem("");

            comboBoxesToDelete.append(comboBox);

            for(int j =0;j<enumParser->enums.size();j++){
                if(enumParser->enums.at(j).name=="0"){
                    continue;
                }
                comboBox->addItem(enumParser->enums.at(j).name);
                comboBox->setItemData(comboBox->count()-1,j,Qt::ToolTipRole);

                if(j==traceWindow->getArgsThatNeedToBeChangedByEnum().value(wID).at(i).enumId){
                    comboBox->setCurrentIndex(comboBox->count()-1);
                }
            }

            //            comboBox->setCurrentIndex(traceWindow->getArgsThatNeedToBeChangedByEnum().value(wID).at(i).enumId);

            ui->rawTracesTable->insertRow(ui->rawTracesTable->rowCount());

            ui->rawTracesTable->setItem(countNumber, 0, new QTableWidgetItem(QString::number(i+1)));
            ui->rawTracesTable->item(countNumber,0)->setCheckState(traceWindow->getArgsThatNeedToBeChangedByEnum().value(wID).at(i).needToShow);
            ui->rawTracesTable->setCellWidget(countNumber,1,comboBox);
        }
        return;
    }



    for(int i =0;i<traceHandler->getUniqueTraces().value(wID).argsID.size();i++){
        if(traceHandler->getUniqueTraces().value(wID).argsID.size()==0){
            return;
        }
        int countNumber = ui->rawTracesTable->rowCount();
        //так как по новой генерируем каждый раз комбобокс, надо запоминать те комбобоксы которые у нас есть и удалять их из памяти
        //после каждого обновления списка
        QComboBox* comboBox = new QComboBox();
        comboBox->addItem("");
        comboBoxesToDelete.append(comboBox);
        for(int i =0;i<enumParser->enums.size();i++){
            if(enumParser->enums.at(i).name=="0"){
                continue;
            }
            comboBox->addItem(enumParser->enums.at(i).name);
            comboBox->setItemData(comboBox->count()-1,i,Qt::ToolTipRole);
        }
        ui->rawTracesTable->insertRow(ui->rawTracesTable->rowCount());

        ui->rawTracesTable->setItem(countNumber, 0, new QTableWidgetItem(QString::number(i+1)));
        ui->rawTracesTable->item(countNumber,0)->setCheckState(Qt::Checked);
        ui->rawTracesTable->setCellWidget(countNumber,1,comboBox);
    }
}

void TraceWindowSettings::loadEnumsFromConfig(){
    //    if(ui->enumsList->count()==0){
    //        QMessageBox msg;
    //        msg.setWindowTitle("Ошибка");
    //        msg.setText("Загрузите файл enum-ов в первую очередь");
    //        msg.exec();
    //        return;
    //    }
    traceWindow->setArgsThatNeedToBeChangedByEnum(config->loadEnums(configSettingsName));
    ui->enumsStatus->setText("Loaded rows from config: "+QString::number(traceWindow->getArgsThatNeedToBeChangedByEnum().size()));
    reloadListOfArgsAndEnums();

    traceWindow->reloadTracesInsideWindow();

}
void TraceWindowSettings::on_loadEnumsFromConfig_clicked()
{
    loadEnumsFromConfig();
}


void TraceWindowSettings::on_clearEnum_clicked()
{
    traceWindow->clearOneEnumElement(ui->traceIDforEnums->currentText().toInt());
    reloadListOfArgsAndEnums();
}


void TraceWindowSettings::on_traceRowBold_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_traceRowItalic_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_debugRowBold_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_debugRowItalic_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_infoRowBold_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_infoRowItalic_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_warningRowBold_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_warningRowItalic_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_errorRowBold_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_errorRowItalic_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_criticalRowBold_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_criticalRowItalic_clicked()
{
    traceWindow->reloadTracesInsideWindow();
}

bool TraceWindowSettings::isTraceBold(){
    return ui->traceRowBold->isChecked();
}

bool TraceWindowSettings::isDebugBold(){
    return ui->debugRowBold->isChecked();
}

bool TraceWindowSettings::isInfoBold(){
    return ui->infoRowBold->isChecked();
}

bool TraceWindowSettings::isWarningBold(){
    return ui->warningRowBold->isChecked();
}

bool TraceWindowSettings::isErrorBold(){
    return ui->errorRowBold->isChecked();
}

bool TraceWindowSettings::isCriticalBold(){
    return ui->criticalRowBold->isChecked();
}

bool TraceWindowSettings::isTraceItalic(){
    return ui->traceRowItalic->isChecked();
}

bool TraceWindowSettings::isDebugItalic(){
    return ui->debugRowItalic->isChecked();
}

bool TraceWindowSettings::isInfoItalic(){;
                                         return ui->infoRowItalic->isChecked();
                                        }

bool TraceWindowSettings::isWarningItalic(){
    return ui->warningRowItalic->isChecked();
}

bool TraceWindowSettings::isErrorItalic(){
    return ui->errorRowItalic->isChecked();
}

bool TraceWindowSettings::isCriticalItalic(){
    return ui->criticalRowItalic->isChecked();
}


void TraceWindowSettings::on_checkAllModules_clicked()
{
    ui->modulesList->blockSignals(true);
    for(int i=0;i<ui->modulesList->count();i++){
        ui->modulesList->item(i)->setCheckState(Qt::CheckState::Checked);
        tUINT16 id = ui->modulesList->item(i)->data(Qt::ToolTipRole).toInt();
        tUINT32 state = ui->modulesList->item(i)->checkState();
        needToShowModules.insert(id,state);
    }
    ui->modulesList->blockSignals(false);
    traceWindow->recountNubmerOfTracesToShow();
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_uncheckAllModules_clicked()
{
    ui->modulesList->blockSignals(true);
    for(int i=0;i<ui->modulesList->count();i++){
        ui->modulesList->item(i)->setCheckState(Qt::CheckState::Unchecked);
        tUINT16 id = ui->modulesList->item(i)->data(Qt::ToolTipRole).toInt();
        tUINT32 state = ui->modulesList->item(i)->checkState();
        needToShowModules.insert(id,state);
    }
    ui->modulesList->blockSignals(false);
    traceWindow->recountNubmerOfTracesToShow();
    traceWindow->reloadTracesInsideWindow();
}

void TraceWindowSettings::on_enumItalicCheckbox_stateChanged(int arg1)
{
    isEnumItalic = ui->enumItalicCheckbox->checkState();
    traceWindow->reloadTracesInsideWindow();
}


void TraceWindowSettings::on_enumBoldCheckbox_stateChanged(int arg1)
{
    isEnumBold = ui->enumBoldCheckbox->checkState();
    traceWindow->reloadTracesInsideWindow();
}

Qt::CheckState TraceWindowSettings::getIsEnumItalic()
{
    return isEnumItalic;
}

Qt::CheckState TraceWindowSettings::getIsEnumBold()
{
    return isEnumBold;
}

Qt::CheckState TraceWindowSettings::isMillisecondsChecked()
{
    return ui->millisecondsCheckbox->checkState();
}

void TraceWindowSettings::on_loadTracesToShowByIdFromConfig_clicked()
{
    loadTracesToShowByIdFromConfig();
}

void TraceWindowSettings::loadTracesToShowByIdFromConfig(){
    config->loadTracesToShowById(configSettingsName);
    for(tUINT32 key:config->getTracesToShowByIdFromConfig().keys()){
        needToShowTraceByID.insert(key,config->getTracesToShowByIdFromConfig().value(key));
    }

    for(int i =0;i<ui->listWidget->count();i++){
        tUINT32 wID = ui->listWidget->item(i)->toolTip().toInt();
        if(needToShowTraceByID.contains(wID)){
            ui->listWidget->item(i)->setCheckState((Qt::CheckState)needToShowTraceByID.value(wID));
        }
    }

    traceWindow->reloadTracesInsideWindow();


}

void TraceWindowSettings::on_saveTracesToShowByIdToConfig_clicked()
{
    config->saveTracesToShowById(configSettingsName,needToShowTraceByID);
}


void TraceWindowSettings::on_loadModulesToShowFromConfig_clicked()
{
    loadModulesToShowFromConfig();
}

void TraceWindowSettings::loadModulesToShowFromConfig(){
    config->loadModulesToShow(configSettingsName);

    for(tUINT32 key:config->getNeedToShowModules().keys()){
        needToShowModules.insert(key,config->getNeedToShowModules().value(key));
    }

    for(int i =0;i<ui->modulesList->count();i++){
        tUINT32 wID = ui->modulesList->item(i)->toolTip().toInt();
        if(needToShowModules.contains(wID)){
            ui->modulesList->item(i)->setCheckState((Qt::CheckState)needToShowModules.value(wID));
        }
    }

    traceWindow->reloadTracesInsideWindow();

}

void TraceWindowSettings::on_saveModulesToShowToConfig_clicked()
{
    config->saveModulesToShow(configSettingsName,needToShowModules);
}

void TraceWindowSettings::loadColumnsFromConfig(){
    QMap<QString, Qt::CheckState>types = config->loadColumnsToShow(configSettingsName);

    //При изменении чекбоксов тригерится стародобавленный сигнал, который обновляет окно traceWindw для перегенерации по "новым условиям"
    //Загрузка происходит до инициализации окна traceWindow, следовательно блочим сигналы перед тем как внести изменения
    ui->sequenceCheckbox->blockSignals(true);
    ui->traceCheckbox->blockSignals(true);
    ui->timeCheckbox->blockSignals(true);
    ui->millisecondsCheckbox->blockSignals(true);
    for(QString type:types.keys()){
        if(type=="sequence"){
            ui->sequenceCheckbox->setCheckState(types.value("sequence"));
        }
        else if(type=="time"){
            ui->timeCheckbox->setCheckState(types.value("time"));
        }else if(type=="trace"){
            ui->traceCheckbox->setCheckState(types.value("trace"));
        }
        else if(type=="milliseconds"){
            ui->millisecondsCheckbox->setCheckState(types.value("milliseconds"));
        }
    }

    ui->sequenceCheckbox->blockSignals(false);
    ui->traceCheckbox->blockSignals(false);
    ui->timeCheckbox->blockSignals(false);
    ui->millisecondsCheckbox->blockSignals(false);

    traceWindow->reloadTracesInsideWindow();
}

tUINT32 TraceWindowSettings::getWheelScrollStep()
{
    return wheelScrollStep;
}


void TraceWindowSettings::on_openLauncher_clicked()
{
    traceWindow->getMainWindow()->showNormal();
    traceWindow->getMainWindow()->raise();
}

void TraceWindowSettings::on_wheelSpin_valueChanged(int arg1)
{
    wheelScrollStep = arg1;

    if(wheelScrollStep==0){
        wheelScrollStep=1;
        ui->wheelSpin->setValue(1);
    }
    config->saveWheelScrollStep(configSettingsName,wheelScrollStep);
}


void TraceWindowSettings::on_millisecondsCheckbox_stateChanged(int arg1)
{
    traceWindow->reloadTracesInsideWindow();

    QMap<QString, Qt::CheckState> typesToShow;
    typesToShow.insert("milliseconds",ui->millisecondsCheckbox->checkState());
    config->saveColumnsToShow(configSettingsName,typesToShow);
}

