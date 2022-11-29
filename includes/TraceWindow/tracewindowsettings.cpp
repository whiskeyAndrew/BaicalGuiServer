#include "tracewindowsettings.h"
#include "ui_tracewindowsettings.h"
#include "tracewindow.h"

<<<<<<< Updated upstream
TraceWindowSettings::TraceWindowSettings(TraceWindow *newTraceWindow):ui(new Ui::TraceWindowSettings)
=======
TraceWindowSettings::TraceWindowSettings(TraceWindow *newTraceWindow, ConnectionName* clientName, ConfigHandler *newConfig):ui(new Ui::TraceWindowSettings)
>>>>>>> Stashed changes
{
    traceWindow = newTraceWindow;
    ui->setupUi(this);
    colorDialog = new QColorDialog();
    connect(ui->listWidget,&QListWidget::itemChanged,this,&TraceWindowSettings::itemChanged);
    connect(this,&TraceWindowSettings::SendRowWID,traceWindow,&TraceWindow::traceRowListCheckboxChanged);
<<<<<<< Updated upstream
}

=======
    colorDialog = new QColorDialog();

    connectionName = *clientName;
    setWindowTitle(clientName->ip+":"+clientName->port +" config");
    config = newConfig;

    InitColors();
    InitTraceLevels();
    LoadConfigFileAsText();
}

void TraceWindowSettings::SetWindowName(QString name){

}

>>>>>>> Stashed changes
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
    rgbFromColorDialog = colorDialog->getColor();
    if(!rgbFromColorDialog.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(rgbFromColorDialog.red())+", "
            +QString::number(rgbFromColorDialog.green())+", "
            +QString::number(rgbFromColorDialog.blue())
            +", 0.2)\"";

    ui->traceColorButton->setStyleSheet("background-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                        +QString::number(rgbFromColorDialog.green())+", "
                                        +QString::number(rgbFromColorDialog.blue())+");"+
                                        "selection-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                        +QString::number(rgbFromColorDialog.green())+", "
                                        +QString::number(rgbFromColorDialog.blue())+");");
    traceWindow->setTraceColor(htmlColor);
}


void TraceWindowSettings::on_debugColorButton_clicked()
{
    rgbFromColorDialog = colorDialog->getColor();
    if(!rgbFromColorDialog.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(rgbFromColorDialog.red())+", "
            +QString::number(rgbFromColorDialog.green())+", "
            +QString::number(rgbFromColorDialog.blue())
            +", 0.2)\"";

    ui->debugColorButton->setStyleSheet("background-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                        +QString::number(rgbFromColorDialog.green())+", "
                                        +QString::number(rgbFromColorDialog.blue())+");"+
                                        "selection-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                        +QString::number(rgbFromColorDialog.green())+", "
                                        +QString::number(rgbFromColorDialog.blue())+");");
    traceWindow->setDebugColor(htmlColor);
}


void TraceWindowSettings::on_infoColorButton_clicked()
{
    rgbFromColorDialog = colorDialog->getColor();
    if(!rgbFromColorDialog.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(rgbFromColorDialog.red())+", "
            +QString::number(rgbFromColorDialog.green())+", "
            +QString::number(rgbFromColorDialog.blue())
            +", 0.2)\"";

    ui->infoColorButton->setStyleSheet("background-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                       +QString::number(rgbFromColorDialog.green())
                                       +", "+QString::number(rgbFromColorDialog.blue())+");"+
                                       "selection-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                       +QString::number(rgbFromColorDialog.green())+", "
                                       +QString::number(rgbFromColorDialog.blue())+");");
    traceWindow->setInfoColor(htmlColor);
}


void TraceWindowSettings::on_warningColorButton_clicked()
{
    rgbFromColorDialog = colorDialog->getColor();
    if(!rgbFromColorDialog.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(rgbFromColorDialog.red())+", "
            +QString::number(rgbFromColorDialog.green())+", "
            +QString::number(rgbFromColorDialog.blue())
            +", 0.2)\"";

    ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                          +QString::number(rgbFromColorDialog.green())+", "
                                          +QString::number(rgbFromColorDialog.blue())+");"+
                                          "selection-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                          +QString::number(rgbFromColorDialog.green())+", "
                                          +QString::number(rgbFromColorDialog.blue())+");");
    traceWindow->setWarningColor(htmlColor);
}


void TraceWindowSettings::on_errorColorButton_clicked()
{
    rgbFromColorDialog = colorDialog->getColor();
    if(!rgbFromColorDialog.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(rgbFromColorDialog.red())+", "
            +QString::number(rgbFromColorDialog.green())+", "
            +QString::number(rgbFromColorDialog.blue())
            +", 0.2)\"";

    ui->errorColorButton->setStyleSheet("background-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                        +QString::number(rgbFromColorDialog.green())+", "
                                        +QString::number(rgbFromColorDialog.blue())+");"+
                                        "selection-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                        +QString::number(rgbFromColorDialog.green())+", "
                                        +QString::number(rgbFromColorDialog.blue())+");");
    traceWindow->setErrorColor(htmlColor);
}


void TraceWindowSettings::on_criticalColorButton_clicked()
{
    rgbFromColorDialog = colorDialog->getColor();
    if(!rgbFromColorDialog.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(rgbFromColorDialog.red())+", "
            +QString::number(rgbFromColorDialog.green())+", "
            +QString::number(rgbFromColorDialog.blue())
            +", 0.2)\"";

    ui->criticalColorButton->setStyleSheet("background-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                           +QString::number(rgbFromColorDialog.green())+", "
                                           +QString::number(rgbFromColorDialog.blue())+");"+
                                           "selection-color: rgb("+QString::number(rgbFromColorDialog.red())+", "
                                           +QString::number(rgbFromColorDialog.green())+", "
                                           +QString::number(rgbFromColorDialog.blue())+");");
    traceWindow->setCriticalColor(htmlColor);
}

<<<<<<< Updated upstream
=======

void TraceWindowSettings::on_warningCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_WARNING,arg1);
    config->warningLevel = static_cast<Qt::CheckState>(arg1);
    config->SaveTraceLevelsToShow();
}


void TraceWindowSettings::on_errorCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_ERROR,arg1);
    config->errorLevel = static_cast<Qt::CheckState>(arg1);
    config->SaveTraceLevelsToShow();
}


void TraceWindowSettings::on_criticalCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_CRITICAL,arg1);
    config->criticalLevel = static_cast<Qt::CheckState>(arg1);
    config->SaveTraceLevelsToShow();
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
    config->SaveColors();
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


void TraceWindowSettings::on_uncheckAllUniqueTraces_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setCheckState(Qt::CheckState::Unchecked);
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
}

void TraceWindowSettings::on_LoadDataFromConfig_clicked()
{
    InitColors();
    InitTraceLevels();
}
>>>>>>> Stashed changes
