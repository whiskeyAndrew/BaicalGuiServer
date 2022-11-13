#include "tracewindowsettings.h"
#include "ui_tracewindowsettings.h"
#include "tracewindow.h"

TraceWindowSettings::TraceWindowSettings(TraceWindow *newTraceWindow):ui(new Ui::TraceWindowSettings)
{
    traceWindow = newTraceWindow;
    ui->setupUi(this);
    colorDialog = new QColorDialog();
    connect(ui->listWidget,&QListWidget::itemChanged,this,&TraceWindowSettings::itemChanged);
    connect(this,&TraceWindowSettings::SendRowWID,traceWindow,&TraceWindow::traceRowListCheckboxChanged);

    InitColors();
    ui->lineEdit->setPlaceholderText("0.0-1.0");
}

void TraceWindowSettings::SetWindowName(QString name){
        this->setWindowTitle(name +" config");
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
    traceColor = colorDialog->getColor();
    if(!traceColor.isValid()){
        return;
    }
    ui->traceColorButton->setStyleSheet("background-color: rgb("+QString::number(traceColor.red())+", "
                                        +QString::number(traceColor.green())+", "
                                        +QString::number(traceColor.blue())+");"+
                                        "selection-color: rgb("+QString::number(traceColor.red())+", "
                                        +QString::number(traceColor.green())+", "
                                        +QString::number(traceColor.blue())+");");
    traceWindow->setTraceColor(traceColor);
}


void TraceWindowSettings::on_debugColorButton_clicked()
{
    debugColor = colorDialog->getColor();
    if(!debugColor.isValid()){
        return;
    }
    ui->debugColorButton->setStyleSheet("background-color: rgb("+QString::number(debugColor.red())+", "
                                        +QString::number(debugColor.green())+", "
                                        +QString::number(debugColor.blue())+");"+
                                        "selection-color: rgb("+QString::number(debugColor.red())+", "
                                        +QString::number(debugColor.green())+", "
                                        +QString::number(debugColor.blue())+");");
    traceWindow->setDebugColor(debugColor);
}


void TraceWindowSettings::on_infoColorButton_clicked()
{
    infoColor = colorDialog->getColor();
    if(!infoColor.isValid()){
        return;
    }
    ui->infoColorButton->setStyleSheet("background-color: rgb("+QString::number(infoColor.red())+", "
                                       +QString::number(infoColor.green())
                                       +", "+QString::number(infoColor.blue())+");"+
                                       "selection-color: rgb("+QString::number(infoColor.red())+", "
                                       +QString::number(infoColor.green())+", "
                                       +QString::number(infoColor.blue())+");");
    traceWindow->setInfoColor(infoColor);
}


void TraceWindowSettings::on_warningColorButton_clicked()
{
    warningColor = colorDialog->getColor();
    if(!warningColor.isValid()){
        return;
    }
    ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(warningColor.red())+", "
                                          +QString::number(warningColor.green())+", "
                                          +QString::number(warningColor.blue())+");"+
                                          "selection-color: rgb("+QString::number(warningColor.red())+", "
                                          +QString::number(warningColor.green())+", "
                                          +QString::number(warningColor.blue())+");");
    traceWindow->setWarningColor(warningColor);
}


void TraceWindowSettings::on_errorColorButton_clicked()
{
    errorColor = colorDialog->getColor();
    if(!errorColor.isValid()){
        return;
    }
    ui->errorColorButton->setStyleSheet("background-color: rgb("+QString::number(errorColor.red())+", "
                                        +QString::number(errorColor.green())+", "
                                        +QString::number(errorColor.blue())+");"+
                                        "selection-color: rgb("+QString::number(errorColor.red())+", "
                                        +QString::number(errorColor.green())+", "
                                        +QString::number(errorColor.blue())+");");
    traceWindow->setErrorColor(errorColor);
}


void TraceWindowSettings::on_criticalColorButton_clicked()
{
    criticalColor = colorDialog->getColor();
    if(!criticalColor.isValid()){
        return;
    }

    ui->criticalColorButton->setStyleSheet("background-color: rgb("+QString::number(criticalColor.red())+", "
                                           +QString::number(criticalColor.green())+", "
                                           +QString::number(criticalColor.blue())+");"+
                                           "selection-color: rgb("+QString::number(criticalColor.red())+", "
                                           +QString::number(criticalColor.green())+", "
                                           +QString::number(criticalColor.blue())+");");
    traceWindow->setCriticalColor(criticalColor);
}


void TraceWindowSettings::on_clearTrace_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Trace", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setTraceColor("");
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
        ui->traceColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearInfo_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Info", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setInfoColor("");
        ui->traceColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearWarning_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Warning", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setWarningColor("");
        ui->traceColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearError_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Error", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setErrorColor("");
        ui->traceColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_clearCritical_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Critical", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setCriticalColor("");
        ui->traceColorButton->setStyleSheet("");
    }
}


void TraceWindowSettings::on_lineEdit_editingFinished()
{
    QRegExp re("[+-]?([0-9]*[.])?[0-9]+");  // a digit (\d), zero or more times (*)
    if (!re.exactMatch(ui->lineEdit->text())){
        ui->lineEdit->setText(traceWindow->getTransparency());
        return;
    }

    if(ui->lineEdit->text().toFloat()>1.0 || ui->lineEdit->text().toFloat()<0.0)
    {
        ui->lineEdit->setText(traceWindow->getTransparency());
        return;
    }

    traceWindow->setTransparency(ui->lineEdit->text());
}

Qt::CheckState TraceWindowSettings::isTraceColumnNeedToShow(){
    return ui->traceCheckbox->checkState();
}

Qt::CheckState TraceWindowSettings::isSequenceColumnNeedToShow(){
    return ui->sequenceCheckbox->checkState();
}
void TraceWindowSettings::InitColors(){
    if(traceWindow->getTraceColor().isValid()){
            ui->traceColorButton->setStyleSheet("background-color: rgb("+QString::number(traceWindow->getTraceColor().red())+", "
                                                +QString::number(traceWindow->getTraceColor().green())+", "
                                                +QString::number(traceWindow->getTraceColor().blue())+");"+
                                                "selection-color: rgb("+QString::number(traceWindow->getTraceColor().red())+", "
                                                +QString::number(traceWindow->getTraceColor().green())+", "
                                                +QString::number(traceWindow->getTraceColor().blue())+");");
        } else{
            ui->traceColorButton->setStyleSheet("");
        }

        if(traceWindow->getDebugColor().isValid()){
            ui->debugColorButton->setStyleSheet("background-color: rgb("+QString::number(traceWindow->getDebugColor().red())+", "
                                                +QString::number(traceWindow->getDebugColor().green())+", "
                                                +QString::number(traceWindow->getDebugColor().blue())+");"+

                                                "selection-color: rgb("+QString::number(traceWindow->getDebugColor().red())+", "
                                                +QString::number(traceWindow->getDebugColor().green())+", "
                                                +QString::number(traceWindow->getDebugColor().blue())+");");
        } else{
            ui->debugColorButton->setStyleSheet("");
        }

        if(traceWindow->getInfoColor().isValid()){
            ui->infoColorButton->setStyleSheet("background-color: rgb("+QString::number(traceWindow->getInfoColor().red())+", "
                                               +QString::number(traceWindow->getInfoColor().green())+", "
                                               +QString::number(traceWindow->getInfoColor().blue())+");"+
                                               "selection-color: rgb("+QString::number(traceWindow->getInfoColor().red())+", "
                                               +QString::number(traceWindow->getInfoColor().green())+", "
                                               +QString::number(traceWindow->getInfoColor().blue())+");");
        } else{
            ui->infoColorButton->setStyleSheet("");
        }

        if(traceWindow->getWarningColor().isValid()){
            ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(traceWindow->getWarningColor().red())+", "
                                                  +QString::number(traceWindow->getWarningColor().green())+", "
                                                  +QString::number(traceWindow->getWarningColor().blue())+");"+
                                                  "selection-color: rgb("+QString::number(traceWindow->getWarningColor().red())+", "
                                                  +QString::number(traceWindow->getWarningColor().green())+", "
                                                  +QString::number(traceWindow->getWarningColor().blue())+");");
        } else{
            ui->warningColorButton->setStyleSheet("");
        }

        if(traceWindow->getErrorColor().isValid()){
            ui->errorColorButton->setStyleSheet("background-color: rgb("+QString::number(traceWindow->getErrorColor().red())+", "
                                                +QString::number(traceWindow->getErrorColor().green())+", "
                                                +QString::number(traceWindow->getErrorColor().blue())+");"+
                                                "selection-color: rgb("+QString::number(traceWindow->getErrorColor().red())+", "
                                                +QString::number(traceWindow->getErrorColor().green())+", "
                                                +QString::number(traceWindow->getErrorColor().blue())+");");
        } else{
            ui->errorColorButton->setStyleSheet("");
        }

        if(traceWindow->getCriticalColor().isValid()){
            ui->criticalColorButton->setStyleSheet("background-color: rgb("+QString::number(traceWindow->getCriticalColor().red())+", "
                                                   +QString::number(traceWindow->getCriticalColor().green())+", "
                                                   +QString::number(traceWindow->getCriticalColor().blue())+");"+
                                                   "selection-color: rgb("+QString::number(traceWindow->getCriticalColor().red())+", "
                                                   +QString::number(traceWindow->getCriticalColor().green())+", "
                                                   +QString::number(traceWindow->getCriticalColor().blue())+");");
        } else{
            ui->criticalColorButton->setStyleSheet("");
        }
}

void TraceWindowSettings::on_traceCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_TRACE,arg1);
}


void TraceWindowSettings::on_debugCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_DEBUG,arg1);
}


void TraceWindowSettings::on_infoCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_INFO,arg1);
}


void TraceWindowSettings::on_warningCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_WARNING,arg1);
}


void TraceWindowSettings::on_errorCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_ERROR,arg1);
}


void TraceWindowSettings::on_criticalCheckBox_stateChanged(int arg1)
{
    traceWindow->changeTraceLevelIsShownElement(EP7TRACE_LEVEL_CRITICAL,arg1);
}


void TraceWindowSettings::on_sequenceCheckbox_stateChanged(int arg1)
{
    if(traceWindow->isAutoscrollCheckd()==Qt::Unchecked){
        traceWindow->ReloadTracesInsideWindow();
    }
}


void TraceWindowSettings::on_traceCheckbox_stateChanged(int arg1)
{
    if(traceWindow->isAutoscrollCheckd()==Qt::Unchecked){
        traceWindow->ReloadTracesInsideWindow();
    }
}

