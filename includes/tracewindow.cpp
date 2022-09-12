#include "ui_tracewindow.h"
#include "tracewindow.h"

TraceWindow::TraceWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceWindow)
{
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
    ui->tableWidget->setItem(countNumber, 0, new QTableWidgetItem(QString::number(trace.dwSequence)));
    ui->tableWidget->item(countNumber,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->tableWidget->setItem(countNumber, 1, new QTableWidgetItem(trace.traceLineReadyToOutput));
    ui->tableWidget->item(countNumber,1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    ui->tableWidget->resizeRowToContents(countNumber);
}

void TraceWindow::GetQueueSize(tUINT32 size)
{
    ui->queueLength->setText(QString::number(size));
}

