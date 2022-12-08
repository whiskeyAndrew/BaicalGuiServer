#include "tracewindowsettings.h"
#include "ui_tracewindowsettings.h"
#include "tracewindow.h"


TraceWindowSettings::TraceWindowSettings(TraceWindow *newTraceWindow, ConnectionName* clientName):ui(new Ui::TraceWindowSettings)
{
    traceWindow = newTraceWindow;
    enumParser = new EnumParser();
    ui->setupUi(this);

    autoTracesCount = ui->autoRowsCounter;

    connect(ui->listWidget,&QListWidget::itemChanged,this,&TraceWindowSettings::itemChanged);
    connect(this,&TraceWindowSettings::SendRowWID,traceWindow,&TraceWindow::traceRowListCheckboxChanged);
    colorDialog = new QColorDialog();

    connectionName = *clientName;
    setWindowTitle(clientName->ip+":"+clientName->port +" config");
    config = new ConfigHandler(connectionName.ip);

    InitTraceLevels();
    InitColors();
    InitWindowsSize();

    QString enumsFile = config->LoadEnumsList(connectionName.ip);
    if(enumsFile!=""){
        LoadEnumsFromFile(enumsFile);
    }

    ui->rowsOnScreen->setValidator(new QIntValidator(0, INT_MAX, this));
}

void TraceWindowSettings::SetWindowName(QString name){

}
TraceWindowSettings::~TraceWindowSettings()
{
    delete ui;
}

void TraceWindowSettings::itemChanged(QListWidgetItem *item)
{
    tUINT32 wID = item->data(Qt::ToolTipRole).toInt();
    tUINT32 state = item->checkState();
    needToShow.insert(wID,state);
    emit SendRowWID(wID,state);
}

void TraceWindowSettings::AppendUniqueTracesList(QString text, tUINT32 wID)
{
    ui->listWidget->addItem(text);
    QListWidgetItem *listItem = ui->listWidget->item(ui->listWidget->count()-1);
    listItem->setData(Qt::ToolTipRole,wID);
    listItem->setCheckState(Qt::Checked);
    needToShow.insert(wID,Qt::Checked);
    ui->traceIDforEnums->addItem(QString::number(wID));
}

void TraceWindowSettings::DisableElement(tUINT32 wID)
{
    needToShow.insert(wID,Qt::Unchecked);
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
        traceWindow->setTraceColor("");
        config->traceColor = "";
        ui->traceColorButton->setStyleSheet("");
    }

}


void TraceWindowSettings::on_clearDebug_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Debug", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setDebugColor("");
        config->debugColor = "";
        ui->debugColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearInfo_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Info", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setInfoColor("");
        config->infoColor = "";
        ui->infoColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearWarning_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Warning", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setWarningColor("");
        config->warningColor = "";
        ui->warningColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearError_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Error", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setErrorColor("");
        config->errorColor = "";
        ui->errorColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearCritical_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Critical", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setCriticalColor("");
        config->criticalColor = "";
        ui->criticalColorButton->setStyleSheet("");
    }
}


Qt::CheckState TraceWindowSettings::isTraceColumnNeedToShow(){
    return ui->traceCheckbox->checkState();
}

Qt::CheckState TraceWindowSettings::isSequenceColumnNeedToShow(){
    return ui->sequenceCheckbox->checkState();
}

void TraceWindowSettings::InitTraceLevels(){
    config->LoadTraceLevelsToShow();
    ui->traceLevelCheckBox->setCheckState(config->traceLevel);
    ui->debugCheckBox->setCheckState(config->debugLevel);
    ui->infoCheckBox->setCheckState(config->infoLevel);
    ui->warningCheckBox->setCheckState(config->warningLevel);
    ui->errorCheckBox->setCheckState(config->errorLevel);
    ui->criticalCheckBox->setCheckState(config->criticalLevel);

}

void TraceWindowSettings::InitColors(){
    config->LoadColors();
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
    ui->horizontalSlider->setValue(config->transparency.toFloat()*100);
    traceWindow->setTransparency(config->transparency);

}

void TraceWindowSettings::on_traceLevelCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_TRACE,arg1);
    config->traceLevel = static_cast<Qt::CheckState>(arg1);
}


void TraceWindowSettings::on_debugCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_DEBUG,arg1);
    config->debugLevel = static_cast<Qt::CheckState>(arg1);
}


void TraceWindowSettings::on_infoCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_INFO,arg1);
    config->infoLevel = static_cast<Qt::CheckState>(arg1);
}


void TraceWindowSettings::on_warningCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_WARNING,arg1);
    config->warningLevel = static_cast<Qt::CheckState>(arg1);
}


void TraceWindowSettings::on_errorCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_ERROR,arg1);
    config->errorLevel = static_cast<Qt::CheckState>(arg1);
}


void TraceWindowSettings::on_criticalCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_CRITICAL,arg1);
    config->criticalLevel = static_cast<Qt::CheckState>(arg1);
}


void TraceWindowSettings::on_sequenceCheckbox_stateChanged(int arg1)
{
    if(traceWindow->isAutoscrollChecked()==Qt::Unchecked){
        traceWindow->ReloadTracesInsideWindow();
    }
}

void TraceWindowSettings::on_traceCheckbox_stateChanged(int arg1)
{
    if(traceWindow->isAutoscrollChecked()==Qt::Unchecked){
        traceWindow->ReloadTracesInsideWindow();
    }
}

void TraceWindowSettings::on_horizontalSlider_sliderReleased()
{
    config->transparency = QString::number(static_cast<float>(ui->horizontalSlider->value())/100);
}

void TraceWindowSettings::on_horizontalSlider_sliderMoved(int position)
{
    traceWindow->setTransparency(QString::number(static_cast<float>(position)/100));
}


void TraceWindowSettings::on_checkAllUniqueTraces_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setCheckState(Qt::CheckState::Checked);
    }
}


void TraceWindowSettings::on_tabWidget_tabBarClicked(int index)
{
    if(index==2){
        LoadConfigFileAsText();
    }
}

void TraceWindowSettings::LoadConfigFileAsText(){
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
    traceWindow->SetActionStatusText("Config was loaded!");
}

void TraceWindowSettings::InitWindowsSize()
{
    config->LoadWindowsSize();
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
        mbx.setText("Filename is empty");
        mbx.exec();
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
    traceWindow->SetActionStatusText("Config was loaded!");
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
    traceWindow->SetActionStatusText("Config was saved!");
}

void TraceWindowSettings::on_saveAsButton_clicked()
{
    QString fileName= QFileDialog::getSaveFileName(this, "Save As",config->getConfigFileName(),tr("Config files(*.ini"));

    if(fileName==""){
        mbx.setText("Filename is empty");
        mbx.exec();
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
    traceWindow->SetActionStatusText("Config was saved!");
}

void TraceWindowSettings::on_LoadDataFromConfig_clicked()
{
    InitColors();
    InitTraceLevels();
}


void TraceWindowSettings::on_uncheckAllUniqueTraces_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setCheckState(Qt::CheckState::Unchecked);
    }
}


void TraceWindowSettings::on_saveAllTraceCheckboxes_clicked()
{
    config->SaveTraceLevelsToShow();
    traceWindow->SetActionStatusText("Traces checkboxes was saved!");
}


void TraceWindowSettings::on_saveAllColors_clicked()
{
    config->SaveColors();
    traceWindow->SetActionStatusText("Traces colors was saved!");
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
    ui->traceSx_size->setText(QString::number(this->size().width()));
    ui->traceSy_size->setText(QString::number(this->size().height()));
}

void TraceWindowSettings::SetTraceWindowSizeText(){
    ui->tracex_size->setText(QString::number(traceWindow->size().width()));
    ui->tracey_size->setText(QString::number(traceWindow->size().height()));
}

QString TraceWindowSettings::getRowsOnScreen()
{
    return ui->rowsOnScreen->text();
}

QCheckBox *TraceWindowSettings::getAutoTracesCount() const
{
    return autoTracesCount;
}




void TraceWindowSettings::on_rowsOnScreen_editingFinished()
{
    traceWindow->recountNumberOfRowsToShow();
}


void TraceWindowSettings::on_saveWindowsProperties_clicked()
{
    config->SaveWindowsSize(traceWindow->size().width(),traceWindow->size().height(),this->size().width(),traceWindow->size().height());
}


void TraceWindowSettings::on_loadEnumsFromTXT_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Save As",nullptr,tr("Text files(*.txt)"));
    LoadEnumsFromFile(fileName);
    traceWindow->traceThread->SetEnumsList(&enumParser->enums);

}

void TraceWindowSettings::LoadEnumsFromFile(QString fileName){
    enumParser->readEnumsFromFile(fileName);
    int enumId = 0;
    ui->enumsList->clear();
    for(int i =0;i<enumParser->enums.size();i++){
        QListWidgetItem *item = new QListWidgetItem(enumParser->enums.at(i).name);
        item->setData(Qt::ToolTipRole,++enumId);
        ui->enumsList->addItem(item);
    }
    config->SaveEnumsList(connectionName.ip,fileName);
}

void TraceWindowSettings::on_enumsList_itemClicked(QListWidgetItem *item)
{
    tUINT32 rowId = item->data(Qt::ToolTipRole).toInt()-1;
    likeEnum _enum = enumParser->enums.at(rowId);
    int enumId = 0;
    ui->enumsElements->clear();

//    for(int i =0;i<_enum.enums.size();i++){
//        QListWidgetItem *item = new QListWidgetItem(_enum.enums.value(i).name+" "+QString::number(_enum.enums.ke));
//        item->setData(Qt::ToolTipRole,++enumId);
//        ui->enumsElements->addItem(item);
//    }

    for(tUINT32 key:_enum.enums.keys()){
        QListWidgetItem *item = new QListWidgetItem(_enum.enums.value(key).name+" "+QString::number(key));
        item->setData(Qt::ToolTipRole,++enumId);
        ui->enumsElements->addItem(item);
    }
    ui->enumId->setText(QString::number(rowId));
}


void TraceWindowSettings::on_applyEnumToTraceById_clicked()
{

    traceWindow->traceThread->AppendTraceThatNeedEnumInsteadOfArgs(ui->traceIDforEnums->currentText().toInt(),ui->enumId->text().toInt());
}

EnumParser *TraceWindowSettings::getEnumParser() const
{
    return enumParser;
}

