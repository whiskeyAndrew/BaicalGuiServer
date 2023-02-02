#include "ui_tracewindow.h"
#include "tracewindow.h"
#include "mainwindow.h"
#include "../FileReader/filereader.h"

//Отображение трейсов на экране. Небольшие возможности редактирования трейсов, отсюда же создается traceWindowSettings, для того чтобы делать настройки отображения трейсов
//Основной алгоритм отображения трейсов расписан ниже, во всем остальном простой набор виджетов с различными сигналами

//Окно отрисовывает текст как HTML. Это было сделано для того, чтобы предоставить приложению вид "консоли",
//Но при этом сохранить байкаловскую возможность кликать по строкам чтобы получать о них информацию.
//Из-за этого вытекло куча проблем, к примеру тэги /n и /t приходится заменять ХТМЛовским аналогом
//Для генерации строк и данных приходится использовать URL-тип строк
//К примеру мы перехватываем ссылку по которой кликнули и получаем "как ссылку" уникальные данные строки, которые генерируем
//в getGuiRow. Для разделения данных (а они исключительно в виде текста передаются) использую ' _splt_ ' поэтому желательно это сочетание не использовать в текстах (костыль)
//Короче генерация строк на экране здесь - это один большой и страшный костыль, альтернативу которому не получается придумать из-за нехватки опыта.
//Проще пытаться придумать альтернативу чем пытаться это все глобально отрефакторить

TraceWindow::TraceWindow(ConnectionName newClientName, ConfigHandler* newConfig, MainWindow* mw, QDialog* parent) :
    QDialog(parent),
    ui(new Ui::TraceWindow)
{
    ui->setupUi(this);

    ui->textBrowser->setUndoRedoEnabled(false);
    mainWindow = mw;
    guiData = new QList<GUIData>;
    clientName = newClientName;
    config = newConfig;

    initWindow();
    initEnded = true;
    ui->actionsStatusLabel->setText("Connected");
}

void TraceWindow::getTrace(GUIData uniqueTrace)
{
    //слишком большая чсть хранить кучу данных в traceTime
    //оптимизировать чуть позже
    uniqueTrace.positionInMap = ui->verticalScrollBar->maximum();
    QString traceRow = getGuiRow(&uniqueTrace);

    guiData->append(uniqueTrace);


    if(isRowNeedToBeShown(uniqueTrace)){
        for(tUINT32 i = 1;i<=uniqueTrace.linesInsideTrace;i++){
            listOfRowsThatWeNeedToShow.append({static_cast<tUINT32>(guiData->size()-1),i});
        }
        ui->verticalScrollBar->setMaximum(ui->verticalScrollBar->maximum()+1);
    }
    if((ui->verticalScrollBar->maximum()<numberOfRowsToShow || ui->textBrowser->document()->blockCount()<numberOfRowsToShow || ui->Autoscroll->isChecked())
            &&isRowNeedToBeShown(uniqueTrace)){
        ui->textBrowser->append(traceRow);
        tracesToReloadCounter++;
        if(tracesToReloadCounter >= TRACES_UNTIL_RELOAD_PAGE){
            //Внутри QTextEdit хранятся какие-то кэшированные данные, которые при работе с ОГРОМНЫМ количеством строк
            //Срут в оперативную память
            //Кажждые TRACES_UNTIL_RELOAD_PAGE чистим и заполняем по новой
            QString text = ui->textBrowser->toPlainText();
            ui->textBrowser->clear();
            ui->textBrowser->append(text);
            std::cout<<"reloaded"<<std::endl;
            tracesToReloadCounter = 0;
        }
        ui->verticalScrollBar->blockSignals(true);
        if(ui->Autoscroll->isChecked()){
            if(ui->textBrowser->document()->blockCount()>numberOfRowsToShow){
                deleteFirstLineInsideTracesWindow();
            }
            ui->verticalScrollBar->setValue(ui->verticalScrollBar->maximum()+1);
            ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
        }
        else{
            ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->minimum());
        }
        ui->verticalScrollBar->blockSignals(false);
    }
}

void TraceWindow::deleteFirstLineInsideTracesWindow()
{
    cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    cursor.deleteChar();
}

void TraceWindow::setSettingsDisabled(bool status)
{
    ui->WindowSettings->setDisabled(status);
}

void TraceWindow::on_verticalScrollBar_valueChanged(int value)
{
    //Если нам надо перестроить все окно, нам надо знать с какой точки его перестраивать, чтобы не переходить в самый ноль
    //Надо запомнить sequence строки и ориентируясь по нему мы поймем когда сделано то, что надо
    if(value<guiData->size() && value<listOfRowsThatWeNeedToShow.size()){
        sequenceToRememberForReloadingAtProperPlace = guiData->at(listOfRowsThatWeNeedToShow.at(value).traceNumber).uniqueData.dwSequence;
    }
    reloadTracesInsideWindow();
}

//Это все какой-то специфичный говнокодик, который я всячески пытаюсь переписать
//Но он отлично работает и доведен до идеала (почти(наверно))
//Попытки его переписать в что-то адекватное не увенчались успехом
//Пока оставляю
void TraceWindow::reloadTracesInsideWindow()
{
    if(!initEnded){
        return;
    }
    tUINT32 value;

    if(!ui->Autoscroll->isChecked()){
        value = ui->verticalScrollBar->value();

    } else{
        value = ui->verticalScrollBar->maximum()-numberOfRowsToShow;
    }

    reloadTracesFromBelow(value);

    if(!ui->Autoscroll->isChecked()){
        //        ui->textBrowser->verticalScrollBar()->setValue(listOfRowsThatWeNeedToShow.at(value).lineNumber-1);
        QTextCursor textCursor = ui->textBrowser->textCursor();
        textCursor.movePosition(QTextCursor::Start);
        if(value<listOfRowsThatWeNeedToShow.size()){
            for(int i =0;i<listOfRowsThatWeNeedToShow.at(value).lineNumber-1;i++){
                textCursor.movePosition(QTextCursor::Down);
                //для более плавного скролла пихаем пустые строки в конец
                ui->textBrowser->append("<br> &nbsp;");
            }
            textCursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            ui->textBrowser->setTextCursor(textCursor);
        }
        else{
            ui->textBrowser->verticalScrollBar()->setValue(0);
        }

    } else{
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
    }
}

void TraceWindow::reloadTracesFromBelow(int value)
{
    ui->textBrowser->setText("");
    bool firstTraceGenerated = false;

    while(ui->textBrowser->document()->blockCount()<numberOfRowsToShow){
        if(value<0){
            ui->textBrowser->setText("");
            value = 0;
        }

        if(value==listOfRowsThatWeNeedToShow.size() ){
            return;
        }
        tUINT32 positionOfTraceInsideGuiDataMap = listOfRowsThatWeNeedToShow.at(value).traceNumber;

        if(value<listOfRowsThatWeNeedToShow.size()){
            if(listOfRowsThatWeNeedToShow.at(value).lineNumber!=1 && firstTraceGenerated){
                value++;
                continue;
            }
        }

        GUIData g = guiData->at(positionOfTraceInsideGuiDataMap);
        QString row = getGuiRow(&g);
        ui->textBrowser->append(row);
        value++;
        if(!firstTraceGenerated){
            firstTraceGenerated = true;
        }
    }

    if(ui->textBrowser->document()->lineCount()<numberOfRowsToShow){
        reloadTracesFromAbove(numberOfRowsToShow);
    }
}

void TraceWindow::reloadTracesFromAbove(int value)
{

    ui->textBrowser->setText("");


    while(ui->textBrowser->document()->blockCount()<numberOfRowsToShow){
        if(value>=listOfRowsThatWeNeedToShow.size() || value<1){
            return;
        }
        tUINT32 positionOfTraceInsideGuiDataMap = listOfRowsThatWeNeedToShow.at(value).traceNumber;
        GUIData g = guiData->at(positionOfTraceInsideGuiDataMap);
        QString row = getGuiRow(&g);
        ui->textBrowser->append(row);
        value--;
    }
}

void TraceWindow::addUniqueTrace(UniqueTraceData trace)
{
    traceSettings->appendUniqueTracesList(trace.traceLineData,trace.traceFormat.wID);
}

void TraceWindow::addModule(sP7Trace_Module module)
{
    traceSettings->appendModules(module);
}

void TraceWindow::openHyperlink(const QUrl &link)
{
    ui->Autoscroll->setChecked(false);
    QString s = link.path();
    //небольшой костыль передачи стринга в линке
    QStringList sl = link.path().split(" _splt_ ");
    tUINT32 sequence = sl.at(0).toInt();
    QString rawTraceWithEnums = sl.at(1);
    tUINT32 positionInGuiDataMap = sl.at(2).toInt();
    lastSelected = sequence;

    sP7Trace_Data traceData = guiData->value(positionInGuiDataMap).uniqueData;
    UniqueTraceData traceFormat = traceThread->getUniqueTraces().value(traceData.wID);

    ui->selectedLabel->setText("Selected row: "+ QString::number(lastSelected));

    if(traceFormat.traceFormat.moduleID!=0)
    {
        ui->moduleID->setText(traceThread->getModule(traceFormat.traceFormat.moduleID));
    } else{
        ui->moduleID->setText("NULL");
    }

    ui->wID->setText(QString::number(traceFormat.traceFormat.wID));
    if(traceFormat.traceFormat.line){
        ui->line->setText(QString::number(traceFormat.traceFormat.line));
    }

    ui->argsLen->setText(QString::number(traceFormat.traceFormat.args_Len));

    ui->bLevel->setText(bLevels.value(traceData.bLevel));
    ui->bProcessor->setText(QString::number(traceData.bProcessor));
    ui->threadID->setText(QString::number(traceData.dwThreadID));
    ui->dwSequence->setText(QString::number(traceData.dwSequence));

    //Нужно добваить игнорирование тэгов
    ui->traceText->setText(traceFormat.traceLineData+"\n\n"+rawTraceWithEnums);
    ui->traceDest->setText(traceFormat.fileDest);
    ui->processName->setText(traceFormat.functionName);


}

void TraceWindow::resizeEvent(QResizeEvent* e)
{
    if(initEnded)
    {
        recountNumberOfRowsToShow();
        traceSettings->setTraceWindowSizeText();
    }
}

void TraceWindow::recountNumberOfRowsToShow()
{
    tUINT32 oldNubmerOfRowsToShow = numberOfRowsToShow;
    if(traceSettings->getAutoTracesCount()->checkState()==Qt::CheckState::Checked){
        numberOfRowsToShow = ui->textBrowser->height()/16;
    } else{
        numberOfRowsToShow = traceSettings->getRowsOnScreen().toInt();
    }

    if(numberOfRowsToShow == oldNubmerOfRowsToShow){
        return;
    }

    reloadTracesInsideWindow();

    //    std::cout<<"Rows on screen: "<<numberOfRowsToShow<<std::endl;
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

void TraceWindow::clearOneEnumElement(tUINT32 wID)
{
    argsThatNeedToBeChangedByEnum.remove(wID);
    reloadTracesInsideWindow();
}

void TraceWindow::setArgsThatNeedToBeChangedByEnum(QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > newArgsThatNeedToBeChangedByEnum)
{
    argsThatNeedToBeChangedByEnum = newArgsThatNeedToBeChangedByEnum;
}

QColor TraceWindow::getEmptyColor()
{
    return emptyColor;
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
        ui->textBrowser->verticalScrollBar()->setValue(0);
        return;
    }else{
        lastSelected=-1;
        ui->selectedLabel->clear();
        //        reloadTracesInsideWindow();
        ui->verticalScrollBar->blockSignals(true);
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->maximum());
        ui->verticalScrollBar->blockSignals(false);
        reloadTracesFromBelow(ui->verticalScrollBar->maximum()-numberOfRowsToShow);
        ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
    }
}

TraceWindow::~TraceWindow()
{
    this->close();
    std::cout<<"------Deleting TraceWindow------"<<std::endl;
    traceSettings->deleteLater();

    //Память не очищается не смотря на то, что мы чистим за собой лист. Зато удаление листа сжирает всю производиловку.
    //Надо будет обязательно исправить
    //    qDebug()<<QDateTime::currentDateTime();
    //    guiData->clear();
    //    listOfRowsThatWeNeedToShow.clear();
    qDebug()<<QDateTime::currentDateTime();
    delete ui;
}

void TraceWindow::mousePressEvent(QMouseEvent* eventPress)
{
    ui->textBrowser->clearFocus();
    ui->selectedLabel->clear();
    lastSelected = -1;
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


void TraceWindow::setTraceAsObject(Trace* trace)
{
    traceThread = trace;
}

void TraceWindow::on_expandButton_clicked(bool checked)
{
    if(checked==true)
    {
        ui->clearSelected->setHidden(true);
        ui->groupBox->setHidden(true);
        ui->Disable->setHidden(true);
        ui->traceTextGroupbox->setHidden(true);
        ui->expandButton->setText("←");
    }
    else
    {
        ui->clearSelected->setHidden(false);
        ui->groupBox->setHidden(false);
        ui->Disable->setHidden(false);
        ui->traceTextGroupbox->setHidden(false);
        ui->expandButton->setText("→");
    }
}

void TraceWindow::setActionStatusText(QString text)
{
    ui->actionsStatusLabel->setText(text);
}

void TraceWindow::setConnectionStatus(tUINT32 status){
    connectionStatus = status;
    if(status==ONLINE){
        QIcon icon;
        icon.addPixmap(QPixmap(":/green-dot.png"), QIcon::Disabled);
        ui->connectionStatus->setIcon(icon);
        ui->actionsStatusLabel->setText("Connected");
    }
    else if(status==UNKNOWN_CONNECTION_STATUS){
        QIcon icon;
        icon.addPixmap(QPixmap(":/yellow-dot.png"), QIcon::Disabled);
        ui->connectionStatus->setIcon(icon);
        ui->actionsStatusLabel->setText("Trying to reconnect");
    }
    else if(status==OFFLINE){
        QIcon icon;
        icon.addPixmap(QPixmap(":/red-dot.png"), QIcon::Disabled);
        ui->connectionStatus->setIcon(icon);
        ui->actionsStatusLabel->setText("Disconnected");
        ui->Autoscroll->setCheckable(false);
        ui->Autoscroll->setChecked(false);
        ui->Autoscroll->setDisabled(true);
    }
    //File
    else if(status ==FILE_CONNECTION){
        ui->Autoscroll->setDisabled(status);
        ui->Autoscroll->setCheckable(!status);
        ui->connectionStatus->setHidden(true);
    }
}

tUINT32 TraceWindow::getConnectionStatus(){
    return connectionStatus;
}

MainWindow *TraceWindow::getMainWindow() const
{
    return mainWindow;
}

void TraceWindow::initWindow(){
    emptyColor.setRgb(0,0,0,255);
    setWindowFlags(Qt::Window);
    setWindowTitle(clientName.ip+":"+clientName.port);
    //Инициализация списка по которому смотрим надо ли показывать трейс по bLevel
    for(int i =0;i<6;i++){
        isNeedToShowByTraceLevel.append(2);
    }
    loadingGif = new QMovie(":/loading.gif");
    traceSettings = new TraceWindowSettings(this,&clientName);
    recountNumberOfRowsToShow();
    ui->textBrowser->setText("");

    ui->verticalScrollBar->setMaximum(0);
    ui->verticalScrollBar->setMinimum(0);
    ui->verticalScrollBar->setPageStep(1);

    QPalette pallete = ui->textBrowser->palette();
    pallete.setColor(QPalette::Active, QPalette::Base, Qt::black);
    pallete.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
    ui->textBrowser->setPalette(pallete);
    ui->textBrowser->setFont(QFont("Courier",10));

    ui->textBrowser->textCursor().setVisualNavigation(false);

    infiniteLine = ui->infinite_line;
    ui->textBrowser->setOpenLinks(false);

    connect(ui->Autoscroll,&QCheckBox::stateChanged,this,&TraceWindow::autoscrollStateChanged);
    //    connect(ui->infinite_line,&QCheckBox::stateChanged,this,&TraceWindow::autoscrollStateChanged);
    connect(ui->verticalScrollBar,&QScrollBar::sliderPressed,this,&TraceWindow::offAutoscroll);
    connect(ui->verticalScrollBar,&QScrollBar::sliderReleased,this,&TraceWindow::verticalSliderReleased);
    connect(ui->textBrowser,&QTextBrowser::anchorClicked,this,&TraceWindow::openHyperlink);

    //По неведомым причинам перехват скролла верх не работает, он все равно скроллит само окно, а не переопределенный слайдер
    //Отключаем
//    ui->textBrowser->verticalScrollBar()->setDisabled(true);
//    ui->textBrowser->verticalScrollBar()->setVisible(false);
    ui->infinite_line->setChecked(true);
    //    ui->traceText->viewport()->setAutoFillBackground(false);
}

void TraceWindow::wheelEvent(QWheelEvent* event)
{
    //При автоскролле работает немного неправильно, надо будет переделать, пока не критично
    QPoint numDegrees = event->angleDelta() / 8*(-1);
    lastSelected = -1;
    ui->selectedLabel->clear();

    /*    if(ui->Autoscroll->isChecked()){
        ui->Autoscroll->setChecked(false);
        reloadTracesFromBelow(ui->verticalScrollBar->value()-numberOfRowsToShow);
        std::cout<<ui->verticalScrollBar->value()<<std::endl;
    }
    else */if(numDegrees.ry()<0){
        //Это для скролла лишних данных
        sliderAction = 2;

        ui->Autoscroll->setChecked(false);
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()-traceSettings->getWheelScrollStep());
    }
    else if(numDegrees.ry()>0){
        ui->Autoscroll->setChecked(false);
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()+traceSettings->getWheelScrollStep());
    }

    reloadTracesInsideWindow();
    event->accept();




}

bool TraceWindow::eventFilter(QObject* object, QEvent* event)
{
    if (object == ui->textBrowser->viewport() && event->type() == QEvent::Wheel) {
        return true;
    }
    return false;
}

void TraceWindow::traceRowListCheckboxChanged()
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
    reloadTracesInsideWindow();
}

void TraceWindow::on_infinite_line_stateChanged(int arg1)
{
    //    reloadTracesInsideWindow();
    if(arg1==Qt::Checked){
        ui->textBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    }
    else{
        ui->textBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::WidgetWidth);
    }

}

QString TraceWindow::getGuiRow(GUIData* g){
    if(!isRowNeedToBeShown(*g)){
        return "";
    }

    //"style=\"background-color:#33475b\""
    QString color= "color:#C0C0C0\">";

    switch(g->uniqueData.bLevel){
    case EP7TRACE_LEVEL_TRACE:
        if(traceColor!=emptyColor&& traceColor.isValid()){
            color = "color:"+traceColor.name()+"\">";
        }
        if(traceSettings->isTraceBold()){
            color.insert(0,"font-weight: bold;");
        }
        if(traceSettings->isTraceItalic()){
            color.insert(0,"font-style: italic;");
        }
        break;

    case EP7TRACE_LEVEL_DEBUG:
        if(debugColor!=emptyColor&& debugColor.isValid()){
            color = "color:"+debugColor.name()+"\">";
        }
        if(traceSettings->isDebugBold()){
            color.insert(0,"font-weight: bold;");
        }
        if(traceSettings->isDebugItalic()){
            color.insert(0,"font-style: italic;");
        }
        break;

    case  EP7TRACE_LEVEL_INFO:
        if(infoColor!=emptyColor && infoColor.isValid()){
            color = "color:"+infoColor.name()+"\">";
        }
        if(traceSettings->isInfoBold()){
            color.insert(0,"font-weight: bold;");
        }
        if(traceSettings->isInfoItalic()){
            color.insert(0,"font-style: italic;");
        }
        break;

    case EP7TRACE_LEVEL_WARNING:
        if(warningColor!=emptyColor&& warningColor.isValid()){
            color = "color:"+warningColor.name()+"\">";
        }
        if(traceSettings->isWarningBold()){
            color.insert(0,"font-weight: bold;");
        }
        if(traceSettings->isWarningItalic()){
            color.insert(0,"font-style: italic;");
        }
        break;

    case EP7TRACE_LEVEL_ERROR:
        if(errorColor!=emptyColor&& errorColor.isValid()){
            color = "color:"+errorColor.name()+"\">";
        }
        if(traceSettings->isErrorBold()){
            color.insert(0,"font-weight: bold;");
        }
        if(traceSettings->isErrorItalic()){
            color.insert(0,"font-style: italic;");
        }
        break;

    case EP7TRACE_LEVEL_CRITICAL:
        if(criticalColor!=emptyColor&& criticalColor.isValid()){
            color = "color:"+criticalColor.name()+"\">";
        }
        if(traceSettings->isCriticalBold()){
            color.insert(0,"font-weight: bold;");
        }
        if(traceSettings->isCriticalItalic()){
            color.insert(0,"font-style: italic;");
        }
        break;
    }

    QString sequenceToGUI = QString::number(g->uniqueData.dwSequence);
    UniqueTraceData uniqueTraceData = traceThread->getUniqueTraces().value(g->uniqueData.wID);
    QList<ArgsPosition> argsPosition;
    QString traceToGUI = traceThread->formatVector(&uniqueTraceData,g->argsValue, &argsPosition);
    QString timeToGUI = "";
    QString traceToRightPanel = traceToGUI;

    if(argsThatNeedToBeChangedByEnum.contains(g->uniqueData.wID)){
        QList<ArgsThatNeedToBeChangedByEnum> args = argsThatNeedToBeChangedByEnum.value(g->uniqueData.wID);
        for(int i =args.size()-1;i>=0;i--){

            //0 - ничего ставить не надо (судя по всему это значение аргумента)
            tUINT32 number = traceToGUI.mid(argsPosition.value(i).argStart,(argsPosition.value(i).argEnd-argsPosition.value(i).argStart)).toInt();
            if(argsThatNeedToBeChangedByEnum.value(g->uniqueData.wID).at(i).enumId==0){
                continue;
            }

            //            if(!traceSettings->enumsIdList.contains(argsThatNeedToBeChangedByEnum.value(g.wID).at(i).enumId)/*&& argsThatNeedToBeChangedByEnum.value(g.wID).value(i).enumId!=1*/){
            //                continue;
            //            }


            //если енамайди больше ста, ты мы используем сгенерированные из файла енамы и надо проверять, есть ли нужный нам енам внутри
            if(!traceSettings->getEnumParser()->enums.at(args.at(i).enumId).enums.contains(number) && argsThatNeedToBeChangedByEnum.value(g->uniqueData.wID).value(i).enumId>100){
                continue;
            }

            QString boldEnumStart = "";
            QString boldEnumEnd = "";
            QString italicEnumStart = "";
            QString italicEnumEnd = "";

            if(traceSettings->getIsEnumBold()==Qt::Checked){
                boldEnumStart = "<b>";
                boldEnumEnd = "</b>";
            }

            if(traceSettings->getIsEnumItalic()==Qt::Checked){
                italicEnumStart = "<i>";
                italicEnumEnd = "</i>";
            }

            //Если мы добавляем уникальные значения енамов из кода, то нам надо обработать этот момент в этом участке кода
            //if(argsThatNeedToBeChangedByEnum.value(g.wID).at(i).enumId==##ENUM_ID##)
            //Все енамы, подгруженные из файла, будут обрабатываться дальше сами, поэтому для добавления своих обработчиков аргументов достаточно просто
            //Указать его айдишник. Айдишник - номер элемента в enumParser по добавлению, начинается с 1, последний не больше 100-а!
            //В блоке case:{ } мы обрабатываем полученное значение нужным нам образом
            //К примеру: аргумент {%I64D}, в digitToGUI мы получаем уже готовое значение аргумента: 500600700800
            //Если у нас выбран модуль 1, то на выходе из кейса значение digitToGUI будет 500 600 700 800
            //Дальше приложение само сделает нужные подстановки, так что, по сути, от разработчика самое главное
            //Просто обработать аргумент в значении digitToGUI

            QString digitToGUI = traceToGUI.mid(argsPosition.value(i).argStart,(argsPosition.value(i).argEnd-argsPosition.value(i).argStart));

            //получаем ID енама и смотрим по свичу куда его применить
            switch(argsThatNeedToBeChangedByEnum.value(g->uniqueData.wID).at(i).enumId){
            case(1):{
                //АЛГОРИТМ ОБРАБОТКИ СТРОКИ
                tUINT32 step = 0;
                for(int i =digitToGUI.length();i>0;i--){
                    if(step==3){
                        digitToGUI.insert(i, " ");
                        step = 0;
                    }
                    step++;
                }
                //~АЛГОРИТМ ОБРАБОТКИ СТРОКИ
                break;
            }

            case(2):{
                //АЛГОРИТМ ОБРАБОТКИ СТРОКИ
                if(digitToGUI.size()<10){
                    continue;
                }
                digitToGUI.insert(digitToGUI.length()-10,".");
                //~АЛГОРИТМ ОБРАБОТКИ СТРОКИ
                break;
            }

                //enumID - енам из файла
            default:{
                if(traceSettings->getEnumParser()->enums.at(args.at(i).enumId).enums.value(number).name!="NULL"){
                    //проверка на случай если значение не подходит под енам
                    tUINT64 digitToGUIInt = digitToGUI.toDouble();
                    if(traceSettings->getEnumParser()->enums.at(args.at(i).enumId).enums.contains(digitToGUIInt)){
                        digitToGUI = traceSettings->getEnumParser()->enums.at(args.at(i).enumId).enums.value(number).name;
                    }
                }
                break;
            }
            }
            if(argsThatNeedToBeChangedByEnum.value(g->uniqueData.wID).at(i).needToShow==Qt::Checked){
                traceToGUI.replace(argsPosition.value(i).argStart,(argsPosition.value(i).argEnd-argsPosition.value(i).argStart),
                                   boldEnumStart+italicEnumStart+digitToGUI+italicEnumEnd+boldEnumEnd);
            }
            traceToRightPanel.replace(argsPosition.value(i).argStart,(argsPosition.value(i).argEnd-argsPosition.value(i).argStart),digitToGUI);
        }
    }

    QString formattedWithEnumGUI = traceToGUI;

    QString sequenceHref = sequenceToGUI+" _splt_ "+traceToRightPanel+" _splt_ "+QString::number(g->positionInMap);
    sequenceHref = sequenceHref.trimmed();

    traceToGUI.insert(0," ");

    if(traceSettings->isSequenceColumnNeedToShow()==Qt::Unchecked){
        sequenceToGUI = "";
    }

    if(traceSettings->isTimeColumnNeedToShow()==Qt::Checked){
        p7Time rawTime = traceThread->countTraceTime(g->uniqueData);
        QString seconds = QString::number(rawTime.dwSeconds);
        if(seconds.length()<2){
            seconds.insert(0,"0");
        }

        QString minutes = QString::number(rawTime.dwMinutes);
        if(minutes.length()<2){
            minutes.insert(0,"0");
        }

        QString hour = QString::number(rawTime.dwHour);
        if(hour.length()<2){
            hour.insert(0,"0");
        }

        timeToGUI = " " + hour+":"+minutes+":"+seconds;

        if(traceSettings->isMillisecondsChecked()==Qt::Checked){
            QString milisec = QString::number(rawTime.dwMilliseconds);
            while(milisec.length()!=3){
                milisec.insert(0,"0");
            }
            timeToGUI.append("."+milisec + " ");
        } else{
            timeToGUI.append(" ");
        }
    }

    if(traceSettings->isTraceColumnNeedToShow()==Qt::Unchecked){
        traceToGUI="";
    }

    //Может быть высокая нагрузка
    //Очередной костыль, который является последствием использования HTML страницы для вывода текста
    //Если мы хотим делать столбцы, нужно добавлять отступ перед следующей строкой если мы делаем перенос
    if(traceToGUI.contains("\n")){
        QString lineBreak = "<br>";
        for(int i =0;i<timeToGUI.length()+sequenceToGUI.length();i++){
            lineBreak.append("&nbsp;");
        }
        traceToGUI.replace("\n",lineBreak);
    }
    //HTML удаляет лишние пробелы, нам это не надо
    traceToGUI.trimmed().replace(" ", "&nbsp;");

    //Замена табуляций
    if(traceToGUI.contains("\t")){
        traceToGUI.replace("\t","&nbsp;&nbsp;&nbsp;&nbsp;");
    }


    //Для того чтобы текст бьыл кликабельным и все выглядело "как в консоли"
    //весь текст отображается в главном окне с помощью ссылок
    //Кликом по тексту мы перехватываем сигнал кути и в методе OpenURL выполняем нужные нам действия
    //В строку мы пишем номер trace-а (sequence) и отформатированный енамом трейс чтобы отобразить его справа
    QString returnableHTMLRow = traceLinkStart+traceLinkHref+sequenceHref
            +traceLinkMiddle+color+sequenceToGUI + timeToGUI
            +traceToGUI+traceLinkEnd;

    return returnableHTMLRow;
}

void TraceWindow::changeTheme(Theme theme)
{
    switch(theme){
    case DARK:{
        this->setStyleSheet("border-color: rgb(0, 0, 0);"
                            "gridline-color: rgb(0, 0, 0);");

        //        ui->groupBox->setStyleSheet("background-color: rgb(0,0,0);"
        //                                    "selection-background-color: rgb(63, 100, 204);"
        //                                    "color:rgb(255,255,255");

        ui->groupBox->setStyleSheet("color: white; background-color: rgb(28,28,28); "
                                    "selection-background-color: rgb(63, 100, 204);");

        ui->groupBox_2->setStyleSheet("color: white; background-color: rgb(28,28,28); "
                                      "selection-background-color: rgb(63, 100, 204);");

        ui->traceTextGroupbox->setStyleSheet("color: white; background-color: rgb(28,28,28); "
                                             "selection-background-color: rgb(63, 100, 204);");

        ui->traceToTxt->setStyleSheet("background-color: rgb(164, 164, 164);");
        ui->WindowSettings->setStyleSheet("background-color: rgb(164, 164, 164);");
        ui->Disable->setStyleSheet("color: white;"
                                   "background-color: rgb(28,28,28);"
                                   "selection-background-color: rgb(63, 100, 204);"
                                   "selection-color: rgb(255, 255, 255); "
                                   "border-color: rgb(0, 0, 0);");

        ui->clearSelected->setStyleSheet("color: white;"
                                         "background-color: rgb(28,28,28);"
                                         "selection-background-color: rgb(63, 100, 204);"
                                         "selection-color: rgb(255, 255, 255); "
                                         "border-color: rgb(0, 0, 0);");
        ui->serverStatus->setStyleSheet("color: white;"
                                        "background-color: rgb(28,28,28);");
        break;
    }

    case DEFAULT:{
        this->setStyleSheet("");
        ui->traceToTxt->setStyleSheet("");
        ui->WindowSettings->setStyleSheet("");
        ui->groupBox->setStyleSheet("");

        ui->groupBox_2->setStyleSheet("");

        ui->traceTextGroupbox->setStyleSheet("");
        ui->Disable->setStyleSheet("");
        ui->clearSelected->setStyleSheet("");

        ui->serverStatus->setStyleSheet("");
        break;
    }
    }
    ui->textBrowser->setStyleSheet("selection-color: rgb(63, 63, 63);"
                                   "color: rgb(255, 255, 255);"
                                   "selection-background-color: rgb(187, 187, 187);"
                                   "background-color: rgb(0,0,0)");
    ui->textBrowser->horizontalScrollBar()->setStyleSheet("");
    ui->verticalScrollBar->setStyleSheet("");

}
void TraceWindow::changeTraceLevelIsShownElement(tUINT32 id, tUINT32 state){
    isNeedToShowByTraceLevel[id] = state;
    if(!ui->Autoscroll->isChecked()){
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
    if(!ui->Autoscroll->isChecked()){
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
    if(!ui->Autoscroll->isChecked()){
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
    if(!ui->Autoscroll->isChecked()){
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
    if(!ui->Autoscroll->isChecked()){
        reloadTracesInsideWindow();
    }
}

QColor TraceWindow::getErrorColor()
{
    return errorColor;
}

void TraceWindow::closeEvent (QCloseEvent *event)
{
    traceSettings->close();
}

void TraceWindow::setErrorColor(QColor newErrorColor)
{
    errorColor = newErrorColor;
    if(!ui->Autoscroll->isChecked()){
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
    if(!ui->Autoscroll->isChecked()){
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
    if(!ui->Autoscroll->isChecked()){
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
    QString fileName;
//    if(guiData->size()>1){
//        p7Time time = guiData->at(1).time;
//        fileName = clientName.ip+"."+clientName.port+"-"+QString::number(time.dwHour)+"."+QString::number(time.dwMinutes)+"."+QString::number(time.dwSeconds);
//    }
//    else{
//        fileName = clientName.ip+"."+clientName.port;
//    }

//    QString filePath = QFileDialog::getSaveFileName(this, "Save As",fileName,tr("Text files(*.txt"));
//    if(filePath==""){
//        return;
//    }

//    TracesToText* traces = new TracesToText(new QList(*guiData),filePath,this);
//    traces->start();
}


void TraceWindow::on_actionsStatusLabel_clicked()
{
    ui->actionsStatusLabel->setText("");
}


void TraceWindow::on_hideServerStatus_clicked()
{
    if(ui->serverStatus->isHidden()){
        ui->hideServerStatus->setText("↓");
        ui->serverStatus->setHidden(false);
    }else{
        ui->hideServerStatus->setText("↑");
        ui->serverStatus->setHidden(true);
    }
}

void TraceWindow::fileReadingStatus(tUINT32 percent){
    if(percent==100){
        fileHasBeenRead = 1;
        ui->fileReadingStatus->clear();
        ui->fileReadingStatus->setPixmap(QPixmap(":/tick.png"));
        ui->fileReadingStatus->setScaledContents(true);
        ui->actionsStatusLabel->setText("File has been read");
    }else{
        fileHasBeenRead = 0;
        ui->connectionStatus->setIcon(QIcon());
        ui->fileReadingStatus->setMovie(loadingGif);
        loadingGif->start();
        ui->actionsStatusLabel->setText("Reading file...");
    }
}

void TraceWindow::setAutoscrollDisabled(bool status)
{
    ui->Autoscroll->setDisabled(status);
    ui->Autoscroll->setCheckable(!status);
}

void TraceWindow::on_verticalScrollBar_sliderPressed()
{
    lastSelected=-1;
    ui->selectedLabel->clear();
}

void TraceWindow::clearSelect()
{
    lastSelected = -1;
    ui->selectedLabel->clear();

    ui->moduleID->clear();

    ui->wID->clear();
    ui->line->clear();

    ui->argsLen->clear();

    ui->bLevel->clear();
    ui->bProcessor->clear();
    ui->threadID->clear();
    ui->dwSequence->clear();

    //Нужно добваить игнорирование тэгов
    ui->traceText->clear();
    ui->traceDest->clear();
    ui->processName->clear();
}

void TraceWindow::on_clearSelected_clicked()
{
    clearSelect();
}

bool TraceWindow::isRowNeedToBeShown(GUIData g)
{
    tUINT32 moduleId = traceThread->getUniqueTraces().value(g.uniqueData.wID).moduleId;

    if(traceSettings->needToShowModules.value(moduleId)!=Qt::Checked){
        return false;
    }

    if(traceSettings->needToShowTraceByID.value(g.uniqueData.wID)!=Qt::Checked){
        return false;
    }


    if(isNeedToShowByTraceLevel.value(g.uniqueData.bLevel)!=Qt::Checked){
        return false;
    }
    return true;
}

void TraceWindow::on_verticalScrollBar_actionTriggered(int action)
{
    ui->selectedLabel->clear();
    lastSelected=-1;
    sliderAction = action;
}

void TraceWindow::recountNubmerOfTracesToShow()
{
    bool isRowWhereWeNeedToStartUpdatingFound = false;
    tUINT32 selectedRowPositionInScrollbar = 0;

    listOfRowsThatWeNeedToShow.clear();
    ui->verticalScrollBar->blockSignals(true);
    ui->verticalScrollBar->setMaximum(0);
    ui->verticalScrollBar->setValue(0);
    ui->verticalScrollBar->blockSignals(false);

    for(int i =0;i<guiData->size();i++){
        if(isRowNeedToBeShown(guiData->at(i))){
            for(tUINT32 j =1;j<=guiData->at(i).linesInsideTrace;j++)
                listOfRowsThatWeNeedToShow.append({static_cast<tUINT32>(i),j});
        }

        //ищем если у нас была выбрана строка до этого
        if(!isRowWhereWeNeedToStartUpdatingFound && guiData->at(i).uniqueData.dwSequence>=lastSelected){
            selectedRowPositionInScrollbar = listOfRowsThatWeNeedToShow.size()-1;
            isRowWhereWeNeedToStartUpdatingFound = true;
            //если строки не было выбрано ищем строку которая была в последний раз перед переделкой страницы
        } else if(!isRowWhereWeNeedToStartUpdatingFound &&lastSelected==-1 && guiData->at(i).uniqueData.dwSequence>=sequenceToRememberForReloadingAtProperPlace){
            selectedRowPositionInScrollbar = listOfRowsThatWeNeedToShow.size()-1;
            isRowWhereWeNeedToStartUpdatingFound = true;

        }
    }


    ui->verticalScrollBar->setMaximum(listOfRowsThatWeNeedToShow.size());
    if(isRowWhereWeNeedToStartUpdatingFound){
        ui->verticalScrollBar->blockSignals(true);
        ui->verticalScrollBar->setValue(selectedRowPositionInScrollbar);
        ui->verticalScrollBar->blockSignals(false);
    }

    std::cout<<"maximum - "<<ui->verticalScrollBar->maximum()<<std::endl;
    std::cout<<"list size - "<<listOfRowsThatWeNeedToShow.size()<<std::endl;
    std::cout<<"guidata size - "<<guiData->size()<<std::endl;
    std::cout<<"value - "<<ui->verticalScrollBar->value()<<std::endl;
    reloadTracesInsideWindow();
}

FileReader *TraceWindow::getFileReader() const
{
    return fileReader;
}

void TraceWindow::setFileReader(FileReader *newFileReader)
{
    fileReader = newFileReader;
}


