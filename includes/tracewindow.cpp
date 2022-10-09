#include "ui_tracewindow.h"
#include "tracewindow.h"
#include <QDateTime>

TraceWindow::TraceWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceWindow)
{
    this->setWindowFlags(Qt::Window);
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(0,50);

    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Text"));

    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->setRowCount(1);

    ui->tableWidget->hideColumn(0);
    ui->tableWidget->hideRow(0);

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}


TraceWindow::~TraceWindow()
{
    delete ui;
}

void TraceWindow::GetTrace(TraceToGUI trace)
{

    int countNumber = ui->tableWidget->rowCount();
    sP7Trace_Data traceData = traceThread->GetTraceData(trace.sequence);

    ui->tableWidget->insertRow(countNumber);
    ui->tableWidget->setItem(countNumber, 0, new QTableWidgetItem(QString::number(trace.sequence)));
    ui->tableWidget->item(countNumber,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->tableWidget->setItem(countNumber, 1, new QTableWidgetItem(trace.trace));
    ui->tableWidget->item(countNumber,1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    switch(traceData.bLevel)
    {
    case EP7TRACE_LEVEL_TRACE:{
        break;
    }

    case EP7TRACE_LEVEL_INFO: {
        ui->tableWidget->item(countNumber,0)->setBackground(QBrush(QColor(176,224,230,70)));
        ui->tableWidget->item(countNumber,1)->setBackground(QBrush(QColor(176,224,230,70)));
        break;
    }

    case EP7TRACE_LEVEL_ERROR:
    {
        ui->tableWidget->item(countNumber,0)->setBackground(QBrush(QColor(255,70,70,70)));
        ui->tableWidget->item(countNumber,1)->setBackground(QBrush(QColor(255,70,70,70)));
        break;
    }

    case EP7TRACE_LEVEL_CRITICAL:
    {
        ui->tableWidget->item(countNumber,0)->setBackground(QBrush(QColor(255,0,0,70)));
        ui->tableWidget->item(countNumber,1)->setBackground(QBrush(QColor(255,0,0,70)));
        break;
    }

    case EP7TRACE_LEVEL_DEBUG:
    {
        ui->tableWidget->item(countNumber,0)->setBackground(QBrush(QColor(255,255,165,70)));
        ui->tableWidget->item(countNumber,1)->setBackground(QBrush(QColor(255,255,165,70)));
        break;
    }
    }

    //ui->tableWidget->item(countNumber,1)->setData(Qt::UserRole,QVariant::fromValue(trace));
    ui->tableWidget->resizeRowToContents(countNumber);
    //ui->tableWidget->scrollToItem(ui->tableWidget->item(countNumber,0),QHeaderView::PositionAtBottom);
}

void TraceWindow::GetQueueSize(tUINT32 size)
{
    //ui->queueLength->setText(QString::number(size));
}

void TraceWindow::GetTraceFromFile(std::queue<TraceToGUI> data){

    ui->tableWidget->setRowCount(data.size());
    int counter = data.size();

    for(int i =0;i<counter;i++){
        TraceToGUI trace = data.front();
        data.pop();
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(trace.sequence)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(trace.trace));
        ui->tableWidget->resizeRowToContents(i);
    }
    this->show();
}

void TraceWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    item = ui->tableWidget->item(item->row(),0);
    sP7Trace_Data traceData = traceThread->GetTraceData(item->text().toInt());
    UniqueTraceData traceFormat = traceThread->GetTraceFormat(traceData.wID);

    if(traceFormat.traceFormat.moduleID!=0)
    {
        ui->moduleID->setText(traceThread->getModule(traceFormat.traceFormat.moduleID));
    } else{
        ui->moduleID->setText("NULL");
    }

    ui->wID->setText(QString::number(traceFormat.traceFormat.wID));
    ui->line->setText(QString::number(traceFormat.traceFormat.line));


    ui->argsLen->setText(QString::number(traceFormat.traceFormat.args_Len));

    ui->bLevel->setText(bLevels.value(traceData.bLevel));
    ui->bProcessor->setText(QString::number(traceData.bProcessor));
    ui->threadID->setText(QString::number(traceData.dwThreadID));
    ui->dwSequence->setText(QString::number(traceData.dwSequence));

    ui->traceText->setText(traceFormat.traceLineData);
    ui->traceDest->setText(traceFormat.fileDest);
    ui->processName->setText(traceFormat.functionName);
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

