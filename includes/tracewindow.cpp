#include "ui_tracewindow.h"
#include "tracewindow.h"
#include <QDateTime>
#include <QStyledItemDelegate>
#include <QColorDialog>
#include <QWheelEvent>
#include <QPlainTextEdit>


TraceWindow::TraceWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceWindow)
{
    this->setWindowFlags(Qt::Window);
    ui->setupUi(this);
    InitWindow();
    connect(ui->Autoscroll,&QCheckBox::stateChanged,this,&TraceWindow::AutoscrollStateChanged);
    connect(ui->infinite_line,&QCheckBox::stateChanged,this,&TraceWindow::AutoscrollStateChanged);
    connect(ui->verticalScrollBar,&QScrollBar::sliderPressed,this,&TraceWindow::OffAutoscroll);
    connect(ui->verticalScrollBar,&QScrollBar::sliderReleased,this,&TraceWindow::VerticalSliderReleased);
}

void TraceWindow::resizeEvent(QResizeEvent* e){
    if(ui->Autoscroll->isChecked()==false){
        for(int i =50;i>0;i--){
            //ui->tableView->resizeRowToContents(i);
        }
    }
}

void TraceWindow::OffAutoscroll(){
    ui->Autoscroll->setChecked(false);
}

void TraceWindow::VerticalSliderReleased(){
    if((ui->verticalScrollBar->value() > ui->verticalScrollBar->maximum() - ui->verticalScrollBar->maximum()*0.05)
            &&(ui->Autoscroll->isChecked()==false)){
        ui->Autoscroll->setChecked(true);
    }
}

void TraceWindow::AutoscrollStateChanged(tUINT32 stat){
    if(ui->infinite_line->isChecked()==false){
        //ui->tableView->resizeRowsToContents();
    }
}
void TraceWindow::onTableClicked(const QModelIndex &index)
{
    ui->Autoscroll->setChecked(false);
    if (index.isValid()) {
        tUINT32 row = index.row();
        //Нулвая колонна скрыта, нам нужен из нее секвенс
        tUINT32 seq = index.siblingAtColumn(0).data().toInt();

        sP7Trace_Data traceData = traceThread->GetTraceData(seq);
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
}

TraceWindow::~TraceWindow()
{
    delete ui;
}

void TraceWindow::mousePressEvent(QMouseEvent *eventPress){
   // ui->tableView->clearSelection();
   // ui->tableView->clearFocus();
}


void TraceWindow::GetTrace(TraceToGUI trace)
{
    //Увеличиваем скроллбар на каждую строку+1
    sP7Trace_Data traceData = traceThread->GetTraceData(trace.sequence);
    UniqueTraceData traceFormat = traceThread->GetTraceFormat(traceData.wID);

    QString time = QString::number(trace.traceTime.dwHour)+":"
            +QString::number(trace.traceTime.dwMinutes)+":"
            +QString::number(trace.traceTime.dwSeconds);

//    if(!traceViewer->needToShowMap.contains(traceData.wID)){

//        traceViewer->needToShowMap.insert(traceData.wID,2);
//        rawTraces->AppendList(traceFormat.traceLineData,2,traceData.wID);
//    }

    ui->plainTextEdit->appendPlainText(QString::number(trace.sequence) + " " + trace.trace);
    /*
    traceViewer->fillTempList(trace.trace,QString::number(trace.sequence),time,traceData.wID);
    ui->verticalScrollBar->setMaximum(traceViewer->getTempText().size()-50);
    ui->verticalScrollBar->setMinimum(0);

    //Первая инициализация при запуске окна
    if(ui->verticalScrollBar->value()==0 && traceViewer->getTempText().size()<50){
        traceViewer->populateData(ui->verticalScrollBar->value());

        //Вот эта штука может вызывать пролаги при большом количестве данных
        //По логике вещей она обновляет ВСЮ таблицу при каждом инсерте, что делать, конечно, неправильно
        //Я в этом не уверен, но 100к данных обновляющихся выдерживает, надо оставить надолго и проверить потом как оно
        //Себя поведет
        traceViewer->UpdateViewer();

        if(ui->infinite_line->isChecked()==false){
         //   ui->tableView->resizeRowsToContents();
        }

    }
    //Последняя строка ресайзится по тексту внутри, остальные строки должны принятьь нужную ширину.
    //Нужно будет перепродумать этот момент

    if(ui->Autoscroll->isChecked()){
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->maximum());
    }
//    ui->tableView->setColumnWidth(0,80);
  //  ui->tableView->setColumnWidth(1,70);*/
}


void TraceWindow::GetTraceFromFile(std::queue<TraceToGUI> data){

    int counter = data.size();

    for(int i =0;i<counter;i++){
        TraceToGUI trace = data.front();
        data.pop();

        //        QString time = QString::number(trace.traceTime.wHour)+":"
        //                +QString::number(trace.traceTime.wMinute)+":"
        //                +QString::number(trace.traceTime.wSecond);
    }
    traceViewer->UpdateViewer();
    this->show();
   // ui->tableView->resizeRowsToContents();
}


void TraceWindow::SetTraceAsObject(Trace *trace)
{
    traceThread = trace;
}

void TraceWindow::ResizeOneRow(tUINT32 rowId){
   // ui->tableView->resizeRowToContents(rowId);
}

void TraceWindow::on_expandButton_clicked(bool checked)
{
    if(checked==true)
    {
        ui->groupBox->setHidden(true);
        ui->expandButton->setText("<-");
    }
    else
    {
        ui->groupBox->setHidden(false);
        ui->expandButton->setText("->");
    }

    if(ui->infinite_line->isChecked()==false){
   //     ui->tableView->resizeRowsToContents();
    }
}

TraceViewer::TraceViewer(QObject *parent) : QAbstractTableModel(parent)
{

}

void TraceViewer::UpdateViewer(){
    //    if(initTime==0){
    //        initTime = GetTickCount();
    //    }
    //    else if(initTime+2500>GetTickCount()){
    //        initTime = 0;
    emit layoutChanged();
    //        if(traceText.length()>50){
    //            for(int i = 50;i>0;i++){
    //                traceWindow->tableView->resizeRowToContents(i);
    //            }
    //        }
    //    }
}

void TraceViewer::populateData(int scrollValue)
{

    /*tUINT32 counter = 50;
    tUINT32 listCursor = scrollValue+50;

    while(true){
        if(counter>50){
            break;
        }

        tINT32 boolean = needToShowMap.value(tempId.value(listCursor),-1);
        if(boolean==0){
            listCursor--;
            continue;
        }
        QString text = tempText.value(listCursor);

        //перенос каждые 80 символов чтоб красиво
        if(traceWindow->infiniteLine->isChecked()==false){
            for(int i = 0;i<text.length();i+=80){
                if(i!=0){
                    text.insert(i,'\n');
                }
            }
        }

        traceText.replace(counter,text);
        traceSequence.replace(counter,tempSequence.value(listCursor));
        traceTimer.replace(counter,tempTimer.value(listCursor));

        counter--;
        listCursor--;
    }

    std::reverse(traceText.begin(),traceText.end());
    std::reverse(traceSequence.begin(),traceSequence.end());
    std::reverse(traceTimer.begin(),traceTimer.end());*/
}

int TraceViewer::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return traceSequence.length();
}

int TraceViewer::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant TraceViewer::data(const QModelIndex &index, int role) const
{
    if (index.column() == 2 && role == Qt::TextAlignmentRole) {
        return Qt::AlignTop;
    }
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return traceSequence[index.row()];
    } else if (index.column() == 1) {
        return traceTimer[index.row()];
    } else if(index.column()==2){
        return traceText[index.row()];
    }
    return QVariant();
}

QVariant TraceViewer::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Sequence");
        } else if (section == 1) {
            return QString("Time");
        }else if (section == 2) {
            return QString("Trace");
        }
    }
    return QVariant();
}

void TraceViewer::initTable()
{
    for(int i =0;i<50;i++){
        traceSequence.append(0);
        traceText.append(0);
        traceTimer.append(0);
    }
}

void TraceViewer::fillTempList(QString text, QString sequence, QString timer,tUINT32 wID)
{
    tempSequence.append(sequence);
    tempTimer.append(timer);
    tempText.append(text);
    tempId.append(wID);
}

void TraceViewer::AppendNeedToShow(tUINT32 wID, bool needToShow)
{
    needToShowMap.insert(wID,needToShow);
}

const QList<QString> &TraceViewer::getTempText() const
{
    return tempText;
}

void TraceViewer::setTraceWindow(TraceWindow *newTraceWindow)
{
    traceWindow = newTraceWindow;
}



void TraceWindow::on_pushButton_clicked()
{
    if(ui->groupBox_3->isVisible()){
        ui->groupBox_3->setVisible(false);
    } else{
        ui->groupBox_3->setVisible(true);
    }
}


void TraceWindow::on_column0_stateChanged(int arg1)
{
    //ui->tableView->setColumnHidden(0,!arg1);
}


void TraceWindow::on_Time_stateChanged(int arg1)
{
   // ui->tableView->setColumnHidden(1,!arg1);
}


void TraceWindow::on_verticalScrollBar_valueChanged(int value)
{       
    traceViewer->populateData(value);
    traceViewer->UpdateViewer();

    if(ui->infinite_line->isChecked()){
   //     ui->tableView->resizeColumnToContents(2);
    }
    else {
     //   ui->tableView->setColumnWidth(2,lastColumnSize);
        for(int i = 20;i>=0;i--){
     //       ui->tableView->resizeRowToContents(i);
        }
    }
  //  ui->tableView->clearSelection();
  //  ui->tableView->clearFocus();

}

void TraceWindow::InitWindow(){
    traceViewer = new TraceViewer(this);
    traceViewer->initTable();
    traceViewer->setTraceWindow(this);
//    ui->tableView->setModel(traceViewer);
 //   ui->tableView->hideColumn(0);
  //  ui->tableView->horizontalHeader()->hide();
    traceViewer->initTable();

  //  ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  //  ui->tableView->horizontalHeader()->setStretchLastSection(true);
  //  ui->tableView->verticalHeader()->setStretchLastSection(true);
  //  ui->tableView->setFont(QFont("Courier",8));
    ui->groupBox_3->setVisible(false);
 //   lastColumnSize = ui->tableView->verticalHeader()->sectionSize(2);
    //Коннект по нажатию на ячейку
 //   connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
    ui->verticalScrollBar->setMaximum(0);
 //   ui->tableView->setItemDelegate(new Delegate);
   //ui->tableView->setColumnHidden(3,true);
  //  ui->tableView->viewport()->installEventFilter(this);

    QPalette pallete = ui->plainTextEdit->palette();
    pallete.setColor(QPalette::Active, QPalette::Base, Qt::black);
    pallete.setColor(QPalette::Inactive, QPalette::Base, Qt::black);

    ui->plainTextEdit->setPalette(pallete);
    ui->plainTextEdit->setFont(QFont("Courier",10));
    rawTraces = new TraceRowsList();
    rawTraces->setTraceWindow(this);
    rawTraces->createConnections();

    infiniteLine = ui->infinite_line;
}

const QMap<tUINT32, bool> &TraceWindow::getNeedToShow() const
{
    return needToShow;
}

void TraceWindow::setClientName(const QString &newClientName)
{
    clientName = newClientName;
    this->setWindowTitle(clientName);
}

void TraceWindow::setStyle(QString newStyleSheet)
{
    setStyleSheet(newStyleSheet);
  //  ui->tableView->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 0, 0);");
}


void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    QString txt = index.model()->data( index, Qt::DisplayRole ).toString();
    painter->setPen(QColor(200,200,200));
    painter->drawText(option.rect,txt);

    QStyleOptionViewItem itemOption(option);
    itemOption.palette.setColor(QPalette::Highlight, Qt::black);  // set your color here
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);

}

void TraceWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8*(-1);
    ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()+numDegrees.y()/15);
    event->accept();
    if(ui->infinite_line->isChecked()==false){
  //      ui->tableView->resizeRowsToContents();
    }
  //  ui->tableView->setViewport(ui->tableView->viewport());
    ui->Autoscroll->setChecked(false);
}

bool TraceWindow::eventFilter(QObject *object, QEvent *event)
{
    //if (object == ui->tableView->viewport() && event->type() == QEvent::Wheel) {
        return true;
    //}
    return false;
}

tUINT64 TraceWindow::getTimer() const
{
    return timer;
}

void TraceWindow::setTimer(tUINT64 newTimer)
{
    timer = newTimer;
}

void TraceWindow::traceRowListCheckboxChanged(tUINT32 wID,tUINT32 state){
    traceViewer->needToShowMap.insert(wID,state);

    traceViewer->populateData(ui->verticalScrollBar->value());
    traceViewer->UpdateViewer();
  //  ui->tableView->resizeRowsToContents();
}

void TraceWindow::on_Disable_clicked()
{
    if(ui->wID->text()==""){
        return;
    }
    else {
        traceViewer->needToShowMap.insert(ui->wID->text().toInt(),0);
    }
    traceViewer->populateData(ui->verticalScrollBar->value());
    traceViewer->UpdateViewer();

    if(ui->infinite_line->isChecked()==false){
      //  ui->tableView->resizeRowsToContents();
    }
}


void TraceWindow::on_infinite_line_stateChanged(int arg1)
{
    on_verticalScrollBar_valueChanged(ui->verticalScrollBar->value());

    if(arg1==Qt::Checked){
        for(int i =0;i<50;i++){
        ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
        }
    }
    else{
        ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);
    }

}


void TraceWindow::on_UniqueRows_clicked()
{
    rawTraces->show();
}

