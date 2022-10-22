#include "tracerowslist.h"
#include "ui_tracerowslist.h"
#include "tracewindow.h"

TraceWindow *TraceRowsList::getTraceWindow() const
{
    return traceWindow;
}

void TraceRowsList::setTraceWindow(TraceWindow *newTraceWindow)
{
    traceWindow = newTraceWindow;
}

TraceRowsList::TraceRowsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceRowsList){
    ui->setupUi(this);
}

TraceRowsList::~TraceRowsList(){
    delete ui;
}

void TraceRowsList::AppendList(QString text, tUINT32 needToShow,tUINT32 wID){
    ui->listWidget->addItem(text);
    QListWidgetItem *listItem = ui->listWidget->item(ui->listWidget->count()-1);
    listItem->setData(Qt::ToolTipRole,wID);
    listItem->setCheckState(Qt::Checked);
}

void TraceRowsList::itemChanged(QListWidgetItem * item){
    tUINT32 wID = item->data(Qt::ToolTipRole).toInt();
    tUINT32 state = item->checkState();
    emit SendRowWID(wID,state);

}

void TraceRowsList::createConnections(){
    connect(ui->listWidget,&QListWidget::itemChanged,this,&TraceRowsList::itemChanged);
    connect(this,&TraceRowsList::SendRowWID,traceWindow,&TraceWindow::traceRowListCheckboxChanged);
}
