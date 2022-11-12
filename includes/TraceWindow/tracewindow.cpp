#include "ui_tracewindow.h"
#include "tracewindow.h"

#define LINES_TO_SHOW 60

TraceWindow::TraceWindow(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::TraceWindow)
{
    this->setWindowFlags(Qt::Window);
    ui->setupUi(this);
    InitWindow();
}

void TraceWindow::AddUniqueTrace(UniqueTraceData trace){
    traceSettings->AppendUniqueTracesList(trace.traceLineData,trace.traceFormat.wID);
}

void TraceWindow::ReloadTracesInsideWindow(){
    tUINT32 counter = 0;
    tUINT32 value = ui->verticalScrollBar->value()+LINES_TO_SHOW;
    ui->textBrowser->setText("");

    while(counter<LINES_TO_SHOW){
        if(value>guiData.size()){
            value = guiData.size()-1;
        }
        if(value<guiData.size()){
            GUIData g = guiData.value(value);
            if(traceSettings->needToShow.value(g.wID)!=Qt::Checked){
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


void TraceWindow::GetTrace(TraceToGUI trace)
{
    ui->verticalScrollBar->setMaximum(guiData.size()-1);
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


void TraceWindow::on_pushButton_clicked()
{
    if(ui->groupBox_3->isVisible()){
        ui->groupBox_3->setVisible(false);
    } else{
        ui->groupBox_3->setVisible(true);
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
        if(traceSettings->needToShow.value(g.wID)!=Qt::Checked){
            return;
        }

        traceText = " "+g.trace;
        sequence = QString::number(g.sequence);

        cursor = ui->textBrowser->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar();
        ui->textBrowser->append(GetGuiRow(g));
        return;
    }

    ui->textBrowser->setText("");

    tUINT32 counter = 0;
    ReloadTracesInsideWindow();

}

void TraceWindow::InitWindow(){
    traceSettings = new TraceWindowSettings(this);

    ui->groupBox_3->setVisible(false);
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

void TraceWindow::setClientName(const QString &newClientName)
{
    clientName = newClientName;
    this->setWindowTitle(clientName);
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
        color = traceColor;
        break;
    case EP7TRACE_LEVEL_DEBUG:
        color = debugColor;
        break;
    case  EP7TRACE_LEVEL_INFO:
        color = infoColor;
        break;
    case EP7TRACE_LEVEL_WARNING:
        color = warningColor;
        break;
    case EP7TRACE_LEVEL_ERROR:
        color = errorColor;
        break;
    case EP7TRACE_LEVEL_CRITICAL:
        color = criticalColor;
        break;
    default:
        break;
    }

    return traceLinkStart+color+traceLinkHref+sequence
            +traceLinkMiddle+sequence
            +traceText+traceLinkEnd;
}


const QString &TraceWindow::getTraceColor() const
{
    return traceColor;
}

void TraceWindow::setTraceColor(const QString &newTraceColor)
{
    traceColor = newTraceColor;
}

const QString &TraceWindow::getDebugColor() const
{
    return debugColor;
}

void TraceWindow::setDebugColor(const QString &newDebugColor)
{
    debugColor = newDebugColor;
}

const QString &TraceWindow::getInfoColor() const
{
    return infoColor;
}

void TraceWindow::setInfoColor(const QString &newInfoColor)
{
    infoColor = newInfoColor;
}

const QString &TraceWindow::getWarningColor() const
{
    return warningColor;
}

void TraceWindow::setWarningColor(const QString &newWarningColor)
{
    warningColor = newWarningColor;
}

const QString &TraceWindow::getErrorColor() const
{
    return errorColor;
}

void TraceWindow::setErrorColor(const QString &newErrorColor)
{
    errorColor = newErrorColor;
}

const QString &TraceWindow::getCriticalColor() const
{
    return criticalColor;
}

void TraceWindow::setCriticalColor(const QString &newCriticalColor)
{
    criticalColor = newCriticalColor;
}

void TraceWindow::on_WindowSettings_clicked()
{
    if(traceSettings->isVisible()){
        traceSettings->raise();
        return;
    }
    traceSettings->show();
}

