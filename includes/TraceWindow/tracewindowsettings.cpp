#include "tracewindowsettings.h"
#include "ui_tracewindowsettings.h"
#include "tracewindow.h"
#include <QObject>

TraceWindowSettings::TraceWindowSettings(TraceWindow* newTraceWindow, ConnectionName* clientName):ui(new Ui::TraceWindowSettings)
{
    ui->setupUi(this);
    traceWindow = newTraceWindow;
    enumParser = new EnumParser();
    connectionName =*clientName;

    initWindow();
}

void TraceWindowSettings::initWindow()
{
    setWindowTitle(connectionName.ip+":"+connectionName.port +" settings");
    ui->traceIDforEnums->addItem("");


    ui->rawTracesTable->insertColumn(0);
    ui->rawTracesTable->setColumnWidth(0, ui->rawTracesTable->width()/10);
    ui->rawTracesTable->insertColumn(1);
    ui->rawTracesTable->horizontalHeader()->setStretchLastSection(true);

    autoTracesCount = ui->autoRowsCounter;

    connect(ui->listWidget,&QListWidget::itemChanged,this,&TraceWindowSettings::itemChanged);
    connect(this,&TraceWindowSettings::sendRowWID,traceWindow,&TraceWindow::traceRowListCheckboxChanged);

    colorDialog = new QColorDialog();
    colorDialog->setOption(QColorDialog::ShowAlphaChannel);
    config = new ConfigHandler(connectionName.ip);

    initTraceLevels();
    initColors();
    initWindowsSize();

    QString enumsFile = config->loadEnumsList(connectionName.ip);
    if(enumsFile!=""){
        if(loadEnumsFromFile(enumsFile)){
            loadEnumsFromConfig();
        }
    }

    ui->rowsOnScreen->setValidator(new QIntValidator(0, INT_MAX, this));
}

TraceWindowSettings::~TraceWindowSettings()
{
    delete ui;
}

void TraceWindowSettings::itemChanged(QListWidgetItem* item)
{
    tUINT32 wID = item->data(Qt::ToolTipRole).toInt();
    tUINT32 state = item->checkState();
    needToShow.insert(wID,state);
    emit sendRowWID(wID,state);
}

void TraceWindowSettings::appendUniqueTracesList(QString text, tUINT32 wID)
{
    ui->listWidget->addItem(text);
    QListWidgetItem* listItem = ui->listWidget->item(ui->listWidget->count()-1);
    listItem->setData(Qt::ToolTipRole,wID);
    listItem->setCheckState(Qt::Checked);
    needToShow.insert(wID,Qt::Checked);
    ui->traceIDforEnums->addItem(QString::number(wID));
}

void TraceWindowSettings::disableElement(tUINT32 wID)
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
        traceWindow->reloadTracesInsideWindow();
    }
}

void TraceWindowSettings::on_traceCheckbox_stateChanged(int arg1)
{
    if(traceWindow->isAutoscrollChecked()==Qt::Unchecked){
        traceWindow->reloadTracesInsideWindow();
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
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setCheckState(Qt::CheckState::Unchecked);
    }
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
    ui->traceSx_size->setText(QString::number(this->size().width()));
    ui->traceSy_size->setText(QString::number(this->size().height()));
}

void TraceWindowSettings::setTraceWindowSizeText(){
    ui->tracex_size->setText(QString::number(traceWindow->size().width()));
    ui->tracey_size->setText(QString::number(traceWindow->size().height()));
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
    int enumId = 1;
    ui->enumsList->clear();
    for(int i =0;i<enumParser->enums.size();i++){
        QListWidgetItem* item = new QListWidgetItem(enumParser->enums.at(i).name);
        item->setData(Qt::ToolTipRole,enumId);
        ui->enumsList->addItem(item);
        enumsIdList.append(enumId);
        enumId++;
    }
    config->saveEnumsList(connectionName.ip,fileName);
    ui->enumsStatus->setText("Loaded enums from " + fileName +": " +QString::number(enumParser->enums.size()));
    return true;
}

void TraceWindowSettings::on_enumsList_itemClicked(QListWidgetItem* item)
{
    tUINT32 rowId = item->data(Qt::ToolTipRole).toInt()-1;
    likeEnum _enum = enumParser->enums.at(rowId);
    int enumId = 0;
    ui->enumsElements->clear();

    //    for(int i =0;i<_enum.enums.size();i++){
    //        QListWidgetItem* item = new QListWidgetItem(_enum.enums.value(i).name+" "+QString::number(_enum.enums.ke));
    //        item->setData(Qt::ToolTipRole,++enumId);
    //        ui->enumsElements->addItem(item);
    //    }

    for(tUINT32 key:_enum.enums.keys()){
        QListWidgetItem* item = new QListWidgetItem(_enum.enums.value(key).name+" "+QString::number(key) +" " +_enum.enums.value(key).comment);
        item->setData(Qt::ToolTipRole,++enumId);
        ui->enumsElements->addItem(item);
    }
    ui->enumId->setText(QString::number(rowId));
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
        tUINT32 enumId = comboBox->currentIndex();
        args.append({argId,enumId,ui->rawTracesTable->item(i,0)->checkState()});
    }

    traceWindow->appendArgsThatNeedToBeChangedByEnum(ui->traceIDforEnums->currentText().toInt(),args);
    ui->enumsStatus->setText("Applied");
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
    ui->enumsStatus->setText("Saved enums: " + QString::number(config->saveEnums(traceWindow->getArgsThatNeedToBeChangedByEnum(),connectionName.ip)));
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

    if(ui->traceIDforEnums->currentText()==""){
        return;
    }

    for(int i =0;i<comboBoxesToDelete.size();i++){
        QComboBox* comboBox = comboBoxesToDelete.at(i);
        delete comboBox;
    }
    comboBoxesToDelete.clear();

    tUINT32 wID = ui->traceIDforEnums->currentText().toInt();
    Trace* traceHandler = traceWindow->traceThread;
    ui->uniqueTraceLabel->setText(traceWindow->traceThread->uniqueTraces.value(wID).traceLineData);

    //Если у нас уже есть изменения по wID, подгружаем их
    if(traceWindow->getArgsThatNeedToBeChangedByEnum().contains(wID)){
        for(int i =0;i<traceHandler->uniqueTraces.value(wID).argsID.size();i++){
            int countNumber = ui->rawTracesTable->rowCount();
            QComboBox* comboBox = new QComboBox();
            comboBox->addItem("");
            comboBoxesToDelete.append(comboBox);

            for(int i =0;i<enumParser->enums.size();i++){
                comboBox->addItem(enumParser->enums.at(i).name);
            }

            comboBox->setCurrentIndex(traceWindow->getArgsThatNeedToBeChangedByEnum().value(wID).at(i).enumId);

            ui->rawTracesTable->insertRow(ui->rawTracesTable->rowCount());

            ui->rawTracesTable->setItem(countNumber, 0, new QTableWidgetItem(QString::number(i+1)));
            ui->rawTracesTable->item(countNumber,0)->setCheckState(traceWindow->getArgsThatNeedToBeChangedByEnum().value(wID).at(i).needToShow);
            ui->rawTracesTable->setCellWidget(countNumber,1,comboBox);
        }
        return;
    }



    for(int i =0;i<traceHandler->uniqueTraces.value(wID).argsID.size();i++){
        if(traceHandler->uniqueTraces.value(wID).argsID.size()==0){
            return;
        }
        int countNumber = ui->rawTracesTable->rowCount();
        //так как по новой генерируем каждый раз комбобокс, надо запоминать те комбобоксы которые у нас есть и удалять их из памяти
        //после каждого обновления списка
        QComboBox* comboBox = new QComboBox();
        comboBox->addItem("");
        comboBoxesToDelete.append(comboBox);
        for(int i =0;i<enumParser->enums.size();i++){
            comboBox->addItem(enumParser->enums.at(i).name);
        }
        ui->rawTracesTable->insertRow(ui->rawTracesTable->rowCount());

        ui->rawTracesTable->setItem(countNumber, 0, new QTableWidgetItem(QString::number(i+1)));
        ui->rawTracesTable->item(countNumber,0)->setCheckState(Qt::Checked);
        ui->rawTracesTable->setCellWidget(countNumber,1,comboBox);
    }
}

void TraceWindowSettings::loadEnumsFromConfig(){
    if(ui->enumsList->count()==0){
        QMessageBox msg;
        msg.setWindowTitle("Ошибка");
        msg.setText("Загрузите файл enum-ов в первую очередь");
        msg.exec();
        return;
    }
    traceWindow->setArgsThatNeedToBeChangedByEnum(config->loadEnums(connectionName.ip));
    ui->enumsStatus->setText("Loaded rows from config: "+QString::number(traceWindow->getArgsThatNeedToBeChangedByEnum().size()));
    reloadListOfArgsAndEnums();

    if(traceWindow->isInitialized()){
        traceWindow->reloadTracesInsideWindow();
    }
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

