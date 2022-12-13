#include "ui_tracewindow.h"
#include "tracewindow.h"

TraceWindow::TraceWindow(ConnectionName newClientName, ConfigHandler* newConfig, QDialog* parent) :
    QDialog(parent),
    ui(new Ui::TraceWindow)
{
    ui->setupUi(this);
    clientName = newClientName;
    config = newConfig;

    initWindow();
    initEnded = true;
}

void TraceWindow::getTrace(TraceToGUI trace)
{
    ui->verticalScrollBar->setMaximum(++verticalBarSize);
    GUIData tempGuiData = {trace.sequence,trace.trace,trace.wID,trace.bLevel,trace.argsPositionAfterFormatting};
    guiData.insert(verticalBarSize,tempGuiData);

    if(verticalBarSize<numberOfRowsToShow){
        ui->textBrowser->verticalScrollBar()->setValue(0);
        QString row = getGuiRow(tempGuiData);
        ui->textBrowser->append(row);
        ui->textBrowser->verticalScrollBar()->setValue(0);
        return;
    }

    if(ui->Autoscroll->isChecked()){
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->maximum());
    }
}

void TraceWindow::on_verticalScrollBar_valueChanged(int value)
{
    reloadTracesInsideWindow();
}


//Это все какой-то специфичный говнокодик, который я всячески пытаюсь переписать
//Но он отлично работает и доведен до идеала (почти(наверно))
//Попытки его переписать в что-то адекватное не увенчались успехом
//Пока оставляю
void TraceWindow::reloadTracesInsideWindow()
{
    tUINT32 value = ui->verticalScrollBar->value();

    if(ui->Autoscroll->isChecked()){

        //не нравится
        if(ui->textBrowser->document()->blockCount()<numberOfRowsToShow && verticalBarSize>numberOfRowsToShow)
        {
            reloadTracesFromAbove(value);
        }
        //нравится

        GUIData g = guiData.value(ui->verticalScrollBar->value());

        if(traceSettings->needToShow.value(g.wID)!=Qt::Checked){
            return;
        }


        if(isNeedToShowByTraceLevel.value(g.bLevel)!=Qt::Checked){
            return;
        }

        ui->textBrowser->append(getGuiRow(g));
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());

        //while - используется для ситуаций, где у нас требуется отображать строки не бесконечной линией
        //мы можем удалять только строки текста, следовательно нужно удалять строки сверху до тех пор, пока их не станет столько, сколько надо
        while(ui->textBrowser->document()->blockCount()>numberOfRowsToShow){
            cursor = ui->textBrowser->textCursor();
            cursor.movePosition(QTextCursor::Start);
            cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
            cursor.select(QTextCursor::LineUnderCursor);
            cursor.removeSelectedText();
            cursor.deleteChar();
        }
        return;
    }

    reloadTracesFromBelow(value);
}

void TraceWindow::reloadTracesFromBelow(int value)
{
    ui->textBrowser->setText("");

    while(ui->textBrowser->document()->blockCount()<numberOfRowsToShow){
        //Выше данных нет
        if(value>verticalBarSize){
            ui->textBrowser->verticalScrollBar()->setValue(0);
            return;
        }

        GUIData g = guiData.value(value);

        if(traceSettings->needToShow.value(g.wID)!=Qt::Checked){
            value++;
            continue;
        }

        if(isNeedToShowByTraceLevel.value(g.bLevel)!=Qt::Checked){
            value++;
            continue;
        }

        ui->textBrowser->append(getGuiRow(g));
        value++;
    }
    ui->textBrowser->verticalScrollBar()->setValue(0);
}

void TraceWindow::reloadTracesFromAbove(int value)
{
    std::cout<<"Reloading from Above"<<std::endl;
    tUINT32 rememberValue = value;
    ui->textBrowser->setText("");

    while(ui->textBrowser->document()->blockCount()<numberOfRowsToShow){
        if(value<1){
            ui->textBrowser->verticalScrollBar()->setValue(0);
            return;
        }

        if(value>verticalBarSize){
            return;
        }

        GUIData g = guiData.value(value);

        if(traceSettings->needToShow.value(g.wID)!=Qt::Checked){
            value--;
            continue;
        }

        if(isNeedToShowByTraceLevel.value(g.bLevel)!=Qt::Checked){
            value--;
            continue;
        }

        ui->textBrowser->moveCursor(QTextCursor::Start);
        ui->textBrowser->insertPlainText("\n");
        ui->textBrowser->insertHtml(getGuiRow(g));
        value--;
    }

    //Удаляем в самом начале отступление
    cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    cursor.deleteChar();

    ui->textBrowser->verticalScrollBar()->setValue(0);
}

void TraceWindow::addUniqueTrace(UniqueTraceData trace)
{
    traceSettings->appendUniqueTracesList(trace.traceLineData,trace.traceFormat.wID);
}

void TraceWindow::openHyperlink(const QUrl &link)
{
    ui->Autoscroll->setChecked(false);
    tUINT32 sequence = link.toString().toInt();

    sP7Trace_Data traceData = traceThread->getTraceData(sequence);
    UniqueTraceData traceFormat = traceThread->getTraceFormat(traceData.wID);

    if(traceFormat.traceFormat.moduleID!=0)
    {
        ui->moduleID->setText(traceThread->getModule(traceFormat.traceFormat.moduleID));
    } else{
        ui->moduleID->setText("NULL");
    }

    ui->wID->setText(QString::number(traceFormat.traceFormat.wID));
    if(traceFormat.traceFormat.line)
        ui->line->setText(QString::number(traceFormat.traceFormat.line));

    ui->argsLen->setText(QString::number(traceFormat.traceFormat.args_Len));

    ui->bLevel->setText(bLevels.value(traceData.bLevel));
    ui->bProcessor->setText(QString::number(traceData.bProcessor));
    ui->threadID->setText(QString::number(traceData.dwThreadID));
    ui->dwSequence->setText(QString::number(traceData.dwSequence));

    //Нужно добваить игнорирование тэгов
    ui->traceText->setText(traceFormat.traceLineData);
    ui->traceDest->setText(traceFormat.fileDest);
    ui->processName->setText(traceFormat.functionName);

}

void TraceWindow::resizeEvent(QResizeEvent* e)
{
    recountNumberOfRowsToShow();
    traceSettings->setTraceWindowSizeText();
}
void TraceWindow::recountNumberOfRowsToShow()
{
    if(traceSettings->getAutoTracesCount()->checkState()==Qt::CheckState::Checked){
        numberOfRowsToShow = ui->textBrowser->height()/16;
    } else{
        numberOfRowsToShow = traceSettings->getRowsOnScreen().toInt();
    }
    reloadTracesInsideWindow();
    std::cout<<"Rows on screen: "<<numberOfRowsToShow<<std::endl;
}

TraceWindowSettings* TraceWindow::getTraceSettings() const
{
    return traceSettings;
}

void TraceWindow::appendArgsThatNeedToBeChangedByEnum(tUINT32 wID, QList<ArgsThatNeedToBeChangedByEnum> args)
{
    argsThatNeedToBeChangedByEnum.insert(wID,args);
    reloadTracesInsideWindow();
}

void TraceWindow::clearArgsThatNeedToBeChangedByEnumm()
{
    argsThatNeedToBeChangedByEnum.clear();
    reloadTracesInsideWindow();
}

QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > TraceWindow::getArgsThatNeedToBeChangedByEnum()
{
    return argsThatNeedToBeChangedByEnum;
}

void TraceWindow::setArgsThatNeedToBeChangedByEnum(QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > newArgsThatNeedToBeChangedByEnum)
{
    argsThatNeedToBeChangedByEnum = newArgsThatNeedToBeChangedByEnum;
}
void TraceWindow::offAutoscroll(){
    ui->Autoscroll->setChecked(false);
}


void TraceWindow::verticalSliderReleased()
{
    if((ui->verticalScrollBar->value() > ui->verticalScrollBar->maximum() - ui->verticalScrollBar->maximum()*0.05)
            &&(ui->Autoscroll->isChecked()==false)){
        ui->Autoscroll->setChecked(true);
    }
}

void TraceWindow::autoscrollStateChanged(tUINT32 stat)
{
    if(stat==Qt::Unchecked){
        return;
    }else{
        reloadTracesInsideWindow();
    }
}

TraceWindow::~TraceWindow()
{
    delete ui;
}

void TraceWindow::mousePressEvent(QMouseEvent* eventPress)
{
    ui->textBrowser->clearFocus();
    ui->Autoscroll->setChecked(false);
}

bool TraceWindow::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent* >(event);
        if (ke->key() == Qt::Key_F11) {
            ui->groupBox_2->setVisible(!ui->groupBox_2->isVisible());
            return true;
        }
    }
    return QWidget::event(event);
}

Qt::CheckState TraceWindow::isAutoscrollChecked()
{
    return ui->Autoscroll->checkState();
}

tBOOL TraceWindow::isInitialized() const
{
    return initEnded;
}

void TraceWindow::getTraceFromFile(std::queue<TraceToGUI> data)
{

}

void TraceWindow::setTraceAsObject(Trace* trace)
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

void TraceWindow::setActionStatusText(QString text)
{
    ui->actionsStatusLabel->setText(text);
}
void TraceWindow::initWindow(){
    setWindowFlags(Qt::Window);
    setWindowTitle(clientName.ip+":"+clientName.port);
    //Инициализация списка по которому смотрим надо ли показывать трейс по bLevel
    for(int i =0;i<6;i++){
        isNeedToShowByTraceLevel.append(2);
    }

    traceSettings = new TraceWindowSettings(this,&clientName);
    recountNumberOfRowsToShow();
    ui->textBrowser->setText("");

    ui->verticalScrollBar->setMaximum(0);
    ui->verticalScrollBar->setMinimum(0);

    QPalette pallete = ui->textBrowser->palette();
    pallete.setColor(QPalette::Active, QPalette::Base, Qt::black);
    pallete.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
    ui->textBrowser->setPalette(pallete);
    ui->textBrowser->setFont(QFont("Courier",10));

    ui->textBrowser->textCursor().setVisualNavigation(false);

    infiniteLine = ui->infinite_line;
    ui->textBrowser->setOpenLinks(false);

    connect(ui->Autoscroll,&QCheckBox::stateChanged,this,&TraceWindow::autoscrollStateChanged);
    connect(ui->infinite_line,&QCheckBox::stateChanged,this,&TraceWindow::autoscrollStateChanged);
    connect(ui->verticalScrollBar,&QScrollBar::sliderPressed,this,&TraceWindow::offAutoscroll);
    connect(ui->verticalScrollBar,&QScrollBar::sliderReleased,this,&TraceWindow::verticalSliderReleased);
    connect(ui->textBrowser,&QTextBrowser::anchorClicked,this,&TraceWindow::openHyperlink);

    //По неведомым причинам перехват скролла верх не работает, он все равно скроллит само окно, а не переопределенный слайдер
    //Отключаем
    ui->textBrowser->verticalScrollBar()->setDisabled(true);
    ui->textBrowser->verticalScrollBar()->setVisible(false);
    ui->infinite_line->setChecked(true);

}

void TraceWindow::setStyle(QString newStyleSheet)
{
    setStyleSheet(newStyleSheet);
}

void TraceWindow::wheelEvent(QWheelEvent* event)
{
    QPoint numDegrees = event->angleDelta() / 8*(-1);
    ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()+numDegrees.y()/15);
    ui->Autoscroll->setChecked(false);
    event->accept();

    ui->textBrowser->setViewport(ui->textBrowser->viewport());

}

bool TraceWindow::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui->textBrowser->viewport() && event->type() == QEvent::Wheel) {
        return true;
    }
    return false;
}

void TraceWindow::traceRowListCheckboxChanged(tUINT32 wID,tUINT32 state)
{
    reloadTracesInsideWindow();
}

void TraceWindow::on_Disable_clicked()
{
    if(ui->dwSequence->text()==""){
        return;
    }
    tUINT32 wID = ui->wID->text().toInt();
    traceSettings->disableElement(wID);

    ui->moduleID->clear();


    ui->wID->clear();
    ui->line->clear();

    ui->argsLen->clear();

    ui->bLevel->clear();
    ui->bProcessor->clear();
    ui->threadID->clear();
    ui->dwSequence->clear();

    ui->traceText->clear();
    ui->traceDest->clear();
    ui->processName->clear();
}

void TraceWindow::on_infinite_line_stateChanged(int arg1)
{
    if(arg1==Qt::Checked){
        ui->textBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    }
    else{
        ui->textBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::WidgetWidth);
    }
}

QString TraceWindow::getGuiRow(GUIData g){
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

    QString sequenceToGUI = QString::number(g.sequence);
    QString traceToGUI = g.trace;

    if(argsThatNeedToBeChangedByEnum.contains(g.wID)){
        QList<ArgsThatNeedToBeChangedByEnum> args = argsThatNeedToBeChangedByEnum.value(g.wID);
        for(int i =args.size()-1;i>=0;i--){

            //0 - ничего ставить не надо
            if(argsThatNeedToBeChangedByEnum.value(g.wID).at(i).enumId==0){
                continue;
            }

            //на случай если айдишника енама нет в списке енамов то скипаем
            if(!traceSettings->enumsIdList.contains(argsThatNeedToBeChangedByEnum.value(g.wID).at(i).enumId)){
                continue;
            }

            tUINT32 number = traceToGUI.mid(g.argsPosition.value(i).argStart,(g.argsPosition.value(i).argEnd-g.argsPosition.value(i).argStart)).toInt();

            if(!traceSettings->getEnumParser()->enums.at(args.at(i).enumId-1).enums.contains(number)){
                continue;
            }
            traceToGUI.replace(g.argsPosition.value(i).argStart,(g.argsPosition.value(i).argEnd-g.argsPosition.value(i).argStart),traceSettings->getEnumParser()->enums.at(args.at(i).enumId-1).enums.value(number).name);
        }
    }

    traceToGUI.insert(0," ");
    if(traceSettings->isSequenceColumnNeedToShow()==Qt::Unchecked){
        sequenceToGUI = "";
    }
    if(traceSettings->isTraceColumnNeedToShow()==Qt::Unchecked){
        traceToGUI="";
    }

    if(traceToGUI.contains("\n")){
        traceToGUI.replace("\n","<br>");
    }
    return traceLinkStart+color+traceLinkHref+sequenceToGUI
            +traceLinkMiddle+sequenceToGUI
            +traceToGUI+traceLinkEnd;
}

void TraceWindow::changeTraceLevelIsShownElement(tUINT32 id, tUINT32 state){
    isNeedToShowByTraceLevel[id] = state;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
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
    //Надо иницализацию проверять, если не проинициализировались то и не перестраиваем ничего
    traceColor = newTraceColor;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
    }
}

QColor TraceWindow::getDebugColor()
{
    return debugColor;
}

void TraceWindow::setDebugColor(QColor newDebugColor)
{
    debugColor = newDebugColor;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
    }
}

QColor TraceWindow::getInfoColor()
{
    return infoColor;
}

void TraceWindow::setInfoColor(QColor newInfoColor)
{
    infoColor = newInfoColor;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
    }
}

QColor TraceWindow::getWarningColor()
{
    return warningColor;
}

void TraceWindow::setWarningColor(QColor newWarningColor)
{
    warningColor = newWarningColor;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
    }
}

QColor TraceWindow::getErrorColor()
{
    return errorColor;
}

void TraceWindow::setErrorColor(QColor newErrorColor)
{
    errorColor = newErrorColor;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
    }
}

QColor TraceWindow::getCriticalColor()
{
    return criticalColor;
}

void TraceWindow::setCriticalColor(QColor newCriticalColor)
{
    criticalColor = newCriticalColor;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
    }
}

QString TraceWindow::getTransparency() const
{
    return transparency;
}

void TraceWindow::setTransparency(QString newTransparency)
{
    transparency = newTransparency;
    if(!ui->Autoscroll->isChecked() && initEnded){
        reloadTracesInsideWindow();
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



void TraceWindow::on_traceToTxt_clicked()
{
    //Поток по окончанию завершается сам по себе и я пока не уверен, надо ли его удалять
    //Надо что-нибудь придумать
    //Получается маленький меморилик
    TracesToText* traces = new TracesToText(new QMap(guiData),this);
    traces->start();
}


void TraceWindow::on_actionsStatusLabel_clicked()
{
    ui->actionsStatusLabel->setText("");
}

