#include "ui_tracewindow.h"
#include "tracewindow.h"

Q_DECLARE_METATYPE(TraceLineData)

TraceWindow::TraceWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceWindow)
{
    this->setWindowFlags(Qt::Window);
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(0,50);
    //    ui->tableWidget->setColumnWidth(1,this->width()-50);

    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Text"));

    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->setRowCount(1);
    ui->tableWidget->hideRow(0);

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);



}


TraceWindow::~TraceWindow()
{
    delete ui;
}

void TraceWindow::GetTrace(TraceLineData trace)
{
    int countNumber = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(countNumber);
    ui->tableWidget->setItem(countNumber, 0, new QTableWidgetItem(QString::number(trace.traceData.dwSequence)));
    ui->tableWidget->item(countNumber,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->tableWidget->setItem(countNumber, 1, new QTableWidgetItem(trace.traceLineToGUI));
    ui->tableWidget->item(countNumber,1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->tableWidget->item(countNumber,1)->setData(Qt::UserRole,QVariant::fromValue(trace));
    ui->tableWidget->resizeRowToContents(countNumber);
    //ui->tableWidget->scrollToItem(ui->tableWidget->item(countNumber,0),QHeaderView::PositionAtBottom);
}

void TraceWindow::GetQueueSize(tUINT32 size)
{
    //ui->queueLength->setText(QString::number(size));
}


void TraceWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    item = ui->tableWidget->item(item->row(),0);
    TraceLineData traceInfo = traceThread->GetTraceDataToGui(item->text().toInt());

    if(traceInfo.traceFormat.moduleID!=0)
    {
        QString moduleName = traceThread->getModule(traceInfo.traceFormat.moduleID);
        ui->moduleID->setText(QString::number(traceInfo.traceFormat.moduleID) + " " + moduleName);
    }

    ui->wID->setText(QString::number(traceInfo.traceFormat.wID));
    ui->line->setText(QString::number(traceInfo.traceFormat.line));


    ui->argsLen->setText(QString::number(traceInfo.traceFormat.args_Len));

    ui->bLevel->setText(QString::number(traceInfo.traceData.bLevel));
    ui->bProcessor->setText(QString::number(traceInfo.traceData.bProcessor));
    ui->threadID->setText(QString::number(traceInfo.traceData.dwThreadID));
    ui->dwSequence->setText(QString::number(traceInfo.traceData.dwSequence));

    ui->traceText->setText(traceInfo.traceLineData);
    ui->traceDest->setText(traceInfo.fileDest);
    ui->processName->setText(traceInfo.functionName);
}

void TraceWindow::SetTraceAsObject(Trace *trace)
{
    traceThread = trace;

}



void TraceWindow::on_expandButton_clicked(bool checked)
{
    if(checked==true)
    {
        ui->groupBox->setMinimumWidth(0);
        ui->groupBox->setMaximumHeight(0);
        ui->expandButton->setText("<-");
    }
    else
    {
        ui->groupBox->setMinimumWidth(280);
        ui->groupBox->setMaximumHeight(16777215);
        ui->expandButton->setText("->");
    }
}

