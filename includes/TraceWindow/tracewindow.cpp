#include "ui_tracewindow.h"
#include "tracewindow.h"

#define LINES_TO_SHOW 100

TraceWindow::TraceWindow(ConnectionName newClientName, ConfigHandler *newConfig, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::TraceWindow)
{
    clientName = newClientName;
    config = newConfig;
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
    this->setWindowTitle(clientName.ip+":"+clientName.port);
    InitWindow();
}


void TraceWindow::AddUniqueTrace(UniqueTraceData trace){
    traceSettings->AppendUniqueTracesList(trace.traceLineData,trace.traceFormat.wID);
}

void TraceWindow::ReloadTracesInsideWindow(){
    tUINT32 counter = 0;
    tUINT32 value = ui->verticalScrollBar->value()+LINES_TO_SHOW-1;
    ui->textBrowser->setText("");

    while(counter<LINES_TO_SHOW){
        if(value>guiData.size()){
            value = guiData.size()-1;
        }

        if(value<guiData.size()){
            GUIData g = guiData.value(value);
            if(value==0){
                return;
            }

            if(traceSettings->needToShow.value(g.wID)!=Qt::Checked){
                value--;
                continue;
            }

            if(isNeedToShowByTraceLevel.at(g.bLevel)!=Qt::Checked){
                value--;
                continue;
            }

            traceText = " "+g.trace;
            sequence = QString::number(g.sequence);
            cursor.movePosition(QTextCursor::Start);
            ui->textBrowser->setTextCursor(cursor);
            //<br> замедляет работу
            //проверка избавляет от тупой пустой строки в самом верху
            if(counter<LINES_TO_SHOW-1){
                ui->textBrowser->insertPlainText("\n");
            }
            ui->textBrowser->insertHtml(GetGuiRow(g));
            value--;
            counter++;
        } else {
            break;
        }

    }

}
void TraceWindow::OpenHyperlink(const QUrl &link){
    ui->Autoscroll->setChecked(false);
    tUINT32 sequence = link.toString().toInt();

    sP7Trace_Data traceData = traceThread->GetTraceData(sequence);
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

    std::cout<<sequence<<std::endl;
}

void TraceWindow::resizeEvent(QResizeEvent* e){
    //    if(ui->Autoscroll->isChecked()==false){
    //        for(int i =50;i>0;i--){
    //            //ui->tableView->resizeRowToContents(i);
    //        }
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
    if(stat!=Qt::Checked){
        return;
    }
    ReloadTracesInsideWindow();
}

TraceWindow::~TraceWindow()
{
    delete ui;
}

void TraceWindow::mousePressEvent(QMouseEvent *eventPress){
    ui->textBrowser->clearFocus();
    ui->Autoscroll->setChecked(false);
}

bool TraceWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_F11) {
            ui->groupBox_2->setVisible(!ui->groupBox_2->isVisible());
            return true;
        }
    }
    return QWidget::event(event);
}

Qt::CheckState TraceWindow::isAutoscrollChecked(){
    return ui->Autoscroll->checkState();
}

void TraceWindow::GetTrace(TraceToGUI trace)
{
    ui->verticalScrollBar->setMaximum(guiData.size());
    guiData.insert(ui->verticalScrollBar->maximum(),{trace.sequence,trace.trace,trace.wID,trace.bLevel});

    if(guiData.size()<LINES_TO_SHOW){
        GUIData g = guiData.value(ui->verticalScrollBar->maximum());

        ui->textBrowser->append(GetGuiRow(g));
    }

    if(ui->Autoscroll->isChecked()){
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->maximum());
    }
}

void TraceWindow::GetTraceFromFile(std::queue<TraceToGUI> data){

}


void TraceWindow::SetTraceAsObject(Trace *trace)
{
    traceThread = trace;
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
}


void TraceWindow::on_verticalScrollBar_valueChanged(int value)
{       
    cursor = ui->textBrowser->textCursor();

    if(value>ui->verticalScrollBar->maximum()-LINES_TO_SHOW){
        ui->Autoscroll->setChecked(true);
    }
    else{
        ui->Autoscroll->setChecked(false);
    }

    if(ui->Autoscroll->isChecked()){
        GUIData g = guiData.value(ui->verticalScrollBar->value());

        if(ui->textBrowser->document()->blockCount()<LINES_TO_SHOW && guiData.size()>LINES_TO_SHOW)
        {
            ReloadTracesInsideWindow();
        }

        while(ui->textBrowser->document()->blockCount()>LINES_TO_SHOW){
            cursor = ui->textBrowser->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
            cursor.select(QTextCursor::LineUnderCursor);
            cursor.removeSelectedText();
            cursor.deleteChar();
        }

        if(traceSettings->needToShow.value(g.wID)!=Qt::Checked){
            return;
        }


        if(isNeedToShowByTraceLevel.value(g.bLevel)!=Qt::Checked){
            return;
        }

        traceText = " "+g.trace;
        sequence = QString::number(g.sequence);


        ui->textBrowser->append(GetGuiRow(g));
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
        return;
    }

    ui->textBrowser->setText("");

    tUINT32 counter = 0;
    ReloadTracesInsideWindow();

}

void TraceWindow::InitWindow(){

    //Инициализация списка по которому смотрим надо ли показывать трейс ПО bLevel
    for(int i =0;i<6;i++){
        isNeedToShowByTraceLevel.append(2);
    }

    traceSettings = new TraceWindowSettings(this,&clientName);

    ui->verticalScrollBar->setMaximum(0);

    QPalette pallete = ui->textBrowser->palette();
    pallete.setColor(QPalette::Active, QPalette::Base, Qt::black);
    pallete.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
    ui->textBrowser->setPalette(pallete);
    ui->textBrowser->setFont(QFont("Courier",10));

    ui->textBrowser->textCursor().setVisualNavigation(false);

    infiniteLine = ui->infinite_line;
    ui->textBrowser->setOpenLinks(false);

    connect(ui->Autoscroll,&QCheckBox::stateChanged,this,&TraceWindow::AutoscrollStateChanged);
    connect(ui->infinite_line,&QCheckBox::stateChanged,this,&TraceWindow::AutoscrollStateChanged);
    connect(ui->verticalScrollBar,&QScrollBar::sliderPressed,this,&TraceWindow::OffAutoscroll);
    connect(ui->verticalScrollBar,&QScrollBar::sliderReleased,this,&TraceWindow::VerticalSliderReleased);
    connect(ui->textBrowser,&QTextBrowser::anchorClicked,this,&TraceWindow::OpenHyperlink);

    //По неведомым причинам перехват скролла верх не работает, он все равно скроллит само окно, а не переопределенный слайдер
    //Отключаем
    ui->textBrowser->verticalScrollBar()->setDisabled(true);
    ui->infinite_line->setChecked(true);

}



void TraceWindow::setStyle(QString newStyleSheet)
{
    setStyleSheet(newStyleSheet);
}

void TraceWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8*(-1);
    ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()+numDegrees.y()/15);
    ui->Autoscroll->setChecked(false);
    event->accept();

    ui->textBrowser->setViewport(ui->textBrowser->viewport());

}

bool TraceWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->textBrowser->viewport() && event->type() == QEvent::Wheel) {
        return true;
    }
    return false;
}


void TraceWindow::traceRowListCheckboxChanged(tUINT32 wID,tUINT32 state){
    //ui->textBrowser->clear();
    on_verticalScrollBar_valueChanged(ui->verticalScrollBar->value());
}

void TraceWindow::on_Disable_clicked()
{
    if(ui->dwSequence->text()==""){
        return;
    }
    tUINT32 wID = ui->wID->text().toInt();
    traceSettings->DisableElement(wID);
}


void TraceWindow::on_infinite_line_stateChanged(int arg1)
{
    on_verticalScrollBar_valueChanged(ui->verticalScrollBar->value());

    if(arg1==Qt::Checked){
        ui->textBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    }
    else{
        ui->textBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::WidgetWidth);
    }
}

QString TraceWindow::GetGuiRow(GUIData g){
    //"style=\"background-color:#33475b\""
    QString color;
    switch(g.bLevel){
    case EP7TRACE_LEVEL_TRACE:
        color = "style=\"background-color:rgba("+QString::number(traceColor.red())+", "
                +QString::number(traceColor.green())+", "
                +QString::number(traceColor.blue())
                +", "+transparency+")\"";
        break;
    case EP7TRACE_LEVEL_DEBUG:
        color = "style=\"background-color:rgba("+QString::number(debugColor.red())+", "
                +QString::number(debugColor.green())+", "
                +QString::number(debugColor.blue())
                +", "+transparency+")\"";
        break;
    case  EP7TRACE_LEVEL_INFO:
        color = "style=\"background-color:rgba("+QString::number(infoColor.red())+", "
                +QString::number(infoColor.green())+", "
                +QString::number(infoColor.blue())
                +", "+transparency+")\"";
        break;
    case EP7TRACE_LEVEL_WARNING:
        color = "style=\"background-color:rgba("+QString::number(warningColor.red())+", "
                +QString::number(warningColor.green())+", "
                +QString::number(warningColor.blue())
                +", "+transparency+")\"";
        break;
    case EP7TRACE_LEVEL_ERROR:
        color = "style=\"background-color:rgba("+QString::number(errorColor.red())+", "
                +QString::number(errorColor.green())+", "
                +QString::number(errorColor.blue())
                +", "+transparency+")\"";
        break;
    case EP7TRACE_LEVEL_CRITICAL:
        color = "style=\"background-color:rgba("+QString::number(criticalColor.red())+", "
                +QString::number(criticalColor.green())+", "
                +QString::number(criticalColor.blue())
                +", "+transparency+")\"";
        break;
    default:
        break;
    }

    QString sequenceToGUI = "";
    if(traceSettings->isSequenceColumnNeedToShow()){
        sequenceToGUI = sequence;
    }
    if(!traceSettings->isTraceColumnNeedToShow()){
        traceText="";
    }

    return traceLinkStart+color+traceLinkHref+sequence
            +traceLinkMiddle+sequenceToGUI
            +traceText+traceLinkEnd;
}

void TraceWindow::changeTraceLevelIsShownElement(tUINT32 id, tUINT32 state){
    isNeedToShowByTraceLevel[id] = state;
    if(ui->Autoscroll->isChecked()==Qt::Unchecked){
        ReloadTracesInsideWindow();
    }
}

const ConnectionName &TraceWindow::getClientName() const
{
    return clientName;
}


QColor TraceWindow::getTraceColor()
{
    return traceColor;
}

void TraceWindow::setTraceColor(QColor newTraceColor)
{
    traceColor = newTraceColor;
    if(!ui->Autoscroll->isChecked()){
        ReloadTracesInsideWindow();
    }
}

QColor TraceWindow::getDebugColor()
{
    return debugColor;
}

void TraceWindow::setDebugColor(QColor newDebugColor)
{
    debugColor = newDebugColor;
    if(!ui->Autoscroll->isChecked()){
        ReloadTracesInsideWindow();
    }
}

QColor TraceWindow::getInfoColor()
{
    return infoColor;
}

void TraceWindow::setInfoColor(QColor newInfoColor)
{
    infoColor = newInfoColor;
    if(!ui->Autoscroll->isChecked()){
        ReloadTracesInsideWindow();
    }
}

QColor TraceWindow::getWarningColor()
{
    return warningColor;
}

void TraceWindow::setWarningColor(QColor newWarningColor)
{
    warningColor = newWarningColor;
    if(!ui->Autoscroll->isChecked()){
        ReloadTracesInsideWindow();
    }
}

QColor TraceWindow::getErrorColor()
{
    return errorColor;
}

void TraceWindow::setErrorColor(QColor newErrorColor)
{
    errorColor = newErrorColor;
    if(!ui->Autoscroll->isChecked()){
        ReloadTracesInsideWindow();
    }
}

QColor TraceWindow::getCriticalColor()
{
    return criticalColor;
}

void TraceWindow::setCriticalColor(QColor newCriticalColor)
{    
    criticalColor = newCriticalColor;
    if(!ui->Autoscroll->isChecked()){
        ReloadTracesInsideWindow();
    }
}

QString TraceWindow::getTransparency() const
{
    return transparency;
}

void TraceWindow::setTransparency(QString newTransparency)
{
    transparency = newTransparency;
    if(!ui->Autoscroll->isChecked()){
        ReloadTracesInsideWindow();
    }
}

void TraceWindow::on_WindowSettings_clicked()
{
    if(traceSettings->isVisible()){
        traceSettings->raise();
        return;
    }
    traceSettings->show();
}

void TracesToText::run()
{
    QFile file(QString::number(QDateTime::currentMSecsSinceEpoch())+".txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
    for(int i=0;i<data->size();i++){
        GUIData dataToFile = data->value(i);
        out<<QString::number(dataToFile.sequence)+" "+ dataToFile.trace;
    }
    file.close();
}

void TraceWindow::on_traceToTxt_clicked()
{
    TracesToText *traces = new TracesToText();
    traces->data = new QMap(guiData);
    traces->start();

    //Не уверен нужно ли удалять поток из памяти, скорее всего надо, чуть позже сделаю
}

