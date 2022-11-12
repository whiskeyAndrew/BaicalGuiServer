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
    QString htmlColor = "style=\"background-color:rgba("+QString::number(traceColor.red())+", "
            +QString::number(traceColor.green())+", "
            +QString::number(traceColor.blue())
            +", "+transparency+")\"";

    ui->traceColorButton->setStyleSheet("background-color: rgb("+QString::number(traceColor.red())+", "
                                        +QString::number(traceColor.green())+", "
                                        +QString::number(traceColor.blue())+");"+
                                        "selection-color: rgb("+QString::number(traceColor.red())+", "
                                        +QString::number(traceColor.green())+", "
                                        +QString::number(traceColor.blue())+");");
    traceWindow->setTraceColor(htmlColor);
}


void TraceWindowSettings::on_debugColorButton_clicked()
{
    debugColor = colorDialog->getColor();
    if(!debugColor.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(debugColor.red())+", "
            +QString::number(debugColor.green())+", "
            +QString::number(debugColor.blue())
            +", "+transparency+")\"";

    ui->debugColorButton->setStyleSheet("background-color: rgb("+QString::number(debugColor.red())+", "
                                        +QString::number(debugColor.green())+", "
                                        +QString::number(debugColor.blue())+");"+
                                        "selection-color: rgb("+QString::number(debugColor.red())+", "
                                        +QString::number(debugColor.green())+", "
                                        +QString::number(debugColor.blue())+");");
    traceWindow->setDebugColor(htmlColor);
}


void TraceWindowSettings::on_infoColorButton_clicked()
{
    infoColor = colorDialog->getColor();
    if(!infoColor.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(infoColor.red())+", "
            +QString::number(infoColor.green())+", "
            +QString::number(infoColor.blue())
            +", "+transparency+")\"";

    ui->infoColorButton->setStyleSheet("background-color: rgb("+QString::number(infoColor.red())+", "
                                       +QString::number(infoColor.green())
                                       +", "+QString::number(infoColor.blue())+");"+
                                       "selection-color: rgb("+QString::number(infoColor.red())+", "
                                       +QString::number(infoColor.green())+", "
                                       +QString::number(infoColor.blue())+");");
    traceWindow->setInfoColor(htmlColor);
}


void TraceWindowSettings::on_warningColorButton_clicked()
{
    warningColor = colorDialog->getColor();
    if(!warningColor.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(warningColor.red())+", "
            +QString::number(warningColor.green())+", "
            +QString::number(warningColor.blue())
            +", "+transparency+")\"";

    ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(warningColor.red())+", "
                                          +QString::number(warningColor.green())+", "
                                          +QString::number(warningColor.blue())+");"+
                                          "selection-color: rgb("+QString::number(warningColor.red())+", "
                                          +QString::number(warningColor.green())+", "
                                          +QString::number(warningColor.blue())+");");
    traceWindow->setWarningColor(htmlColor);
}


void TraceWindowSettings::on_errorColorButton_clicked()
{
    errorColor = colorDialog->getColor();
    if(!errorColor.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(errorColor.red())+", "
            +QString::number(errorColor.green())+", "
            +QString::number(errorColor.blue())
            +", "+transparency+")\"";

    ui->errorColorButton->setStyleSheet("background-color: rgb("+QString::number(errorColor.red())+", "
                                        +QString::number(errorColor.green())+", "
                                        +QString::number(errorColor.blue())+");"+
                                        "selection-color: rgb("+QString::number(errorColor.red())+", "
                                        +QString::number(errorColor.green())+", "
                                        +QString::number(errorColor.blue())+");");
    traceWindow->setErrorColor(htmlColor);
}


void TraceWindowSettings::on_criticalColorButton_clicked()
{
    criticalColor = colorDialog->getColor();
    if(!criticalColor.isValid()){
        return;
    }
    QString htmlColor = "style=\"background-color:rgba("+QString::number(criticalColor.red())+", "
            +QString::number(criticalColor.green())+", "
            +QString::number(criticalColor.blue())
            +", "+transparency+")\"";

    ui->criticalColorButton->setStyleSheet("background-color: rgb("+QString::number(criticalColor.red())+", "
                                           +QString::number(criticalColor.green())+", "
                                           +QString::number(criticalColor.blue())+");"+
                                           "selection-color: rgb("+QString::number(criticalColor.red())+", "
                                           +QString::number(criticalColor.green())+", "
                                           +QString::number(criticalColor.blue())+");");
    traceWindow->setCriticalColor(htmlColor);
}


void TraceWindowSettings::on_clearTrace_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Trace", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setTraceColor("");
        //добавить изменение цвета кнопка
    }
}


void TraceWindowSettings::on_clearDebug_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Debug", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setDebugColor("");
        //добавить изменение цвета кнопка
    }
}


void TraceWindowSettings::on_clearInfo_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Info", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setInfoColor("");
        //добавить изменение цвета кнопка
    }
}


void TraceWindowSettings::on_clearWarning_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Warning", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setWarningColor("");
        //добавить изменение цвета кнопка
    }
}


void TraceWindowSettings::on_clearError_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Error", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setErrorColor("");
        //добавить изменение цвета кнопка
    }
}


void TraceWindowSettings::on_clearCritical_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Critical", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        traceWindow->setCriticalColor("");
        //добавить изменение цвета кнопка
    }
}


void TraceWindowSettings::on_lineEdit_editingFinished()
{
    QRegExp re("[+-]?([0-9]*[.])?[0-9]+");  // a digit (\d), zero or more times (*)
    if (!re.exactMatch(ui->lineEdit->text())){
        ui->lineEdit->setText(transparency);
        return;
    }

    if(ui->lineEdit->text().toFloat()>1.0 || ui->lineEdit->text().toFloat()<0.0)
    {
        ui->lineEdit->setText(transparency);
        return;
    }

    transparency = ui->lineEdit->text();

    ReloadColors();
}

void TraceWindowSettings::ReloadColors(){
    QString htmlColor;
    if(traceColor.isValid()){
        htmlColor = "style=\"background-color:rgba("+QString::number(traceColor.red())+", "
                +QString::number(traceColor.green())+", "
                +QString::number(traceColor.blue())
                +", "+transparency+")\"";

        ui->traceColorButton->setStyleSheet("background-color: rgb("+QString::number(traceColor.red())+", "
                                            +QString::number(traceColor.green())+", "
                                            +QString::number(traceColor.blue())+");"+
                                            "selection-color: rgb("+QString::number(traceColor.red())+", "
                                            +QString::number(traceColor.green())+", "
                                            +QString::number(traceColor.blue())+");");
        traceWindow->setTraceColor(htmlColor);
    }

    if(debugColor.isValid()){
        htmlColor = "style=\"background-color:rgba("+QString::number(debugColor.red())+", "
                +QString::number(debugColor.green())+", "
                +QString::number(debugColor.blue())
                +", "+transparency+")\"";

        ui->debugColorButton->setStyleSheet("background-color: rgb("+QString::number(debugColor.red())+", "
                                            +QString::number(debugColor.green())+", "
                                            +QString::number(debugColor.blue())+");"+
                                            "selection-color: rgb("+QString::number(debugColor.red())+", "
                                            +QString::number(debugColor.green())+", "
                                            +QString::number(debugColor.blue())+");");
        traceWindow->setDebugColor(htmlColor);
    }

    if(infoColor.isValid()){
        htmlColor = "style=\"background-color:rgba("+QString::number(infoColor.red())+", "
                +QString::number(infoColor.green())+", "
                +QString::number(infoColor.blue())
                +", "+transparency+")\"";

        ui->infoColorButton->setStyleSheet("background-color: rgb("+QString::number(infoColor.red())+", "
                                           +QString::number(infoColor.green())
                                           +", "+QString::number(infoColor.blue())+");"+
                                           "selection-color: rgb("+QString::number(infoColor.red())+", "
                                           +QString::number(infoColor.green())+", "
                                           +QString::number(infoColor.blue())+");");
        traceWindow->setInfoColor(htmlColor);
    }

    if(warningColor.isValid()){
        htmlColor = "style=\"background-color:rgba("+QString::number(warningColor.red())+", "
                +QString::number(warningColor.green())+", "
                +QString::number(warningColor.blue())
                +", "+transparency+")\"";

        ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(warningColor.red())+", "
                                              +QString::number(warningColor.green())+", "
                                              +QString::number(warningColor.blue())+");"+
                                              "selection-color: rgb("+QString::number(warningColor.red())+", "
                                              +QString::number(warningColor.green())+", "
                                              +QString::number(warningColor.blue())+");");
        traceWindow->setWarningColor(htmlColor);
    }

    if(errorColor.isValid()){
        htmlColor = "style=\"background-color:rgba("+QString::number(errorColor.red())+", "
                +QString::number(errorColor.green())+", "
                +QString::number(errorColor.blue())
                +", "+transparency+")\"";

        ui->errorColorButton->setStyleSheet("background-color: rgb("+QString::number(errorColor.red())+", "
                                            +QString::number(errorColor.green())+", "
                                            +QString::number(errorColor.blue())+");"+
                                            "selection-color: rgb("+QString::number(errorColor.red())+", "
                                            +QString::number(errorColor.green())+", "
                                            +QString::number(errorColor.blue())+");");
        traceWindow->setErrorColor(htmlColor);
    }

    if(warningColor.isValid()){
        htmlColor = "style=\"background-color:rgba("+QString::number(warningColor.red())+", "
                +QString::number(warningColor.green())+", "
                +QString::number(warningColor.blue())
                +", "+transparency+")\"";

        ui->warningColorButton->setStyleSheet("background-color: rgb("+QString::number(warningColor.red())+", "
                                              +QString::number(warningColor.green())+", "
                                              +QString::number(warningColor.blue())+");"+
                                              "selection-color: rgb("+QString::number(warningColor.red())+", "
                                              +QString::number(warningColor.green())+", "
                                              +QString::number(warningColor.blue())+");");
        traceWindow->setWarningColor(htmlColor);
    }
}
