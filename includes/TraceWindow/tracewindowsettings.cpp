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

