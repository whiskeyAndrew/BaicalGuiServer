#include "ui_tracewindow.h"
#include "tracewindow.h"

//Окно отрисовывает текст как HTML. Это было сделано для того, чтобы предоставить приложению вид "консоли",
//Но при этом сохранить байкаловскую возможность кликать по строкам чтобы получать о них информацию.
//Из-за этого вытекло куча проблем, к примеру тэги /n и /t приходится заменять ХТМЛовским аналогом
//Для генерации строк и данных приходится использовать URL-тип строк
//К примеру мы перехватываем ссылку по которой кликнули и получаем "как ссылку" уникальные данные строки, которые генерируем
//в getGuiRow. Для разделения данных (а они исключительно в виде текста передаются) использую ' _splt_ ' поэтому желательно это сочетание не использовать в текстах (костыль)
//Короче генерация строк на экране здесь - это один большой и страшный костыль, альтернативу которому не получается придумать из-за нехватки опыта.
//Проще пытаться придумать альтернативу чем пытаться это все глобально отрефакторить


TraceWindow::TraceWindow(ConnectionName newClientName, ConfigHandler* newConfig, QDialog* parent) :
    QDialog(parent),
    ui(new Ui::TraceWindow)
{
    ui->setupUi(this);
    clientName = newClientName;
    config = newConfig;

    initWindow();
    initEnded = true;
    ui->actionsStatusLabel->setText("Connected");
}

void TraceWindow::getTrace(TraceToGUI trace)
{
    ui->verticalScrollBar->setMaximum(++verticalBarSize);

    //слишком большая часть хранить кучу данных в traceTime
    //оптимизировать чуть позже
    GUIData tempGuiData = {trace.sequence,trace.trace,trace.wID,trace.bLevel,trace.argsPositionAfterFormatting,trace.traceTime,ui->verticalScrollBar->maximum()};
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
    tUINT32 value;
    if(lastSelected==-1){
        value = ui->verticalScrollBar->value();
    }
    else{
        ui->verticalScrollBar->setValue(lastSelected);
        value = lastSelected;
    }

    //костыльный фикс неприятного бага с повторяющейся последней строчкой при отключении соединения
    if(value == lastScrollValue && ui->Autoscroll->isChecked() && verticalBarSize>100){
        return;
    }
    else{
        lastScrollValue = value;
    }

    if(ui->Autoscroll->isChecked()){

        //не нравится
        if(ui->textBrowser->document()->blockCount()<numberOfRowsToShow && verticalBarSize>numberOfRowsToShow)
        {
            reloadTracesFromAbove(value);
        }
        //нравится

        GUIData g = guiData.value(ui->verticalScrollBar->value());

        tUINT32 moduleId = traceThread->uniqueTraces.value(g.wID).moduleId;
        if(traceSettings->needToShowModules.value(moduleId)!=Qt::Checked){
            return;
        }

        if(traceSettings->needToShowTraceByID.value(g.wID)!=Qt::Checked){
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
    //    std::cout<<value<<std::endl;

    std::cout<<sliderAction<<std::endl;
    //Переменные для скипа пустых строк
    tBOOL foundFirstRow = false;
    tINT32 rowsSkipped = 0;
    tUINT32 rememberValue = value;
    //~Переменные для скипа пустых строк

    while(ui->textBrowser->document()->blockCount()<numberOfRowsToShow){
        //Выше данных нет
        if(value>verticalBarSize){
            ui->textBrowser->verticalScrollBar()->setValue(0);
            return;
        }

        GUIData g = guiData.value(value);

        tUINT32 moduleId = traceThread->uniqueTraces.value(g.wID).moduleId;
        if(traceSettings->needToShowModules.value(moduleId)!=Qt::Checked){
            //Генерация строк идет с верхней и вниз.
            //Если мы поднимаемся наверх по скроллу, нам надо найти ту строку, которая нам подходит для генерации по условиям
            //И начинать генерацию с нее
            //Поэтому мы ловим действие, нажатое в скролле с помощью сигнала (sliderAction)
            //И если мы поднимались вверх, то нам надо делать отступ вверх до тех пор, пока мы не найдем нужную строку
            //Затем стандартно генерируем сверху вниз нужные строки
            //В конец передвигаем бегунок на нужную нам позицию
            if(!foundFirstRow && (sliderAction==QAbstractSlider::SliderSingleStepSub || sliderAction==QAbstractSlider::SliderPageStepSub)){
                value--;
                rowsSkipped--;
                continue;
            }

            if(!foundFirstRow){
                rowsSkipped++;
            }
            value++;
            continue;
        }

        if(traceSettings->needToShowTraceByID.value(g.wID)!=Qt::Checked){
            if(!foundFirstRow && (sliderAction==QAbstractSlider::SliderSingleStepSub || sliderAction==QAbstractSlider::SliderPageStepSub)){
                value--;
                rowsSkipped--;
                continue;
            }

            if(!foundFirstRow){
                rowsSkipped++;
            }
            value++;
            continue;
        }

        if(isNeedToShowByTraceLevel.value(g.bLevel)!=Qt::Checked){
            if(!foundFirstRow && (sliderAction==QAbstractSlider::SliderSingleStepSub || sliderAction==QAbstractSlider::SliderPageStepSub)){
                value--;
                rowsSkipped--;
                continue;
            }

            if(!foundFirstRow){
                rowsSkipped++;
            }
            value++;
            continue;
        }

        foundFirstRow = true;
        ui->textBrowser->append(getGuiRow(g));
        value++;
    }
    ui->verticalScrollBar->blockSignals(true);
    ui->verticalScrollBar->setValue(rememberValue+rowsSkipped);
    ui->verticalScrollBar->blockSignals(false);
    sliderAction = 0;
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
        tUINT32 moduleId = traceThread->uniqueTraces.value(g.wID).moduleId;

        if(traceSettings->needToShowModules.value(moduleId)!=Qt::Checked){
            value--;
            continue;
        }

        if(traceSettings->needToShowTraceByID.value(g.wID)!=Qt::Checked){
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
    QString rawTrace = sl.at(1);
    lastSelected = sl.at(2).toInt();

    sP7Trace_Data traceData = traceThread->getTraceData(sequence);
    UniqueTraceData traceFormat = traceThread->getTraceFormat(traceData.wID);

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
    ui->traceText->setPlainText(traceFormat.traceLineData+"\n\n"+rawTrace);
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
        return;
    }else{
        lastSelected=-1;
        ui->selectedLabel->clear();
        reloadTracesInsideWindow();
    }
}

TraceWindow::~TraceWindow()
{
    std::cout<<"------Deleting TraceWindow------"<<std::endl;
    traceSettings->deleteLater();
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
        ui->Disable->setHidden(true);
        ui->traceTextGroupbox->setHidden(true);
        ui->expandButton->setText("←");
    }
    else
    {
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
    if(status==2){
        QIcon icon;
        icon.addPixmap(QPixmap(":/green-dot.png"), QIcon::Disabled);
        ui->connectionStatus->setIcon(icon);
        ui->actionsStatusLabel->setText("Connected");
    }
    else if(status==1){
        QIcon icon;
        icon.addPixmap(QPixmap(":/yellow-dot.png"), QIcon::Disabled);
        ui->connectionStatus->setIcon(icon);
        ui->actionsStatusLabel->setText("Trying to reconnect");
    }
    else if(status==0){
        QIcon icon;
        icon.addPixmap(QPixmap(":/red-dot.png"), QIcon::Disabled);
        ui->connectionStatus->setIcon(icon);
        ui->actionsStatusLabel->setText("Disconnected");
    }
}

tUINT32 TraceWindow::getConnectionStatus(){
    return connectionStatus;
}

void TraceWindow::initWindow(){
    emptyColor.setRgb(0,0,0,255);
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
    connect(ui->infinite_line,&QCheckBox::stateChanged,this,&TraceWindow::autoscrollStateChanged);
    connect(ui->verticalScrollBar,&QScrollBar::sliderPressed,this,&TraceWindow::offAutoscroll);
    connect(ui->verticalScrollBar,&QScrollBar::sliderReleased,this,&TraceWindow::verticalSliderReleased);
    connect(ui->textBrowser,&QTextBrowser::anchorClicked,this,&TraceWindow::openHyperlink);

    //По неведомым причинам перехват скролла верх не работает, он все равно скроллит само окно, а не переопределенный слайдер
    //Отключаем
    ui->textBrowser->verticalScrollBar()->setDisabled(true);
    ui->textBrowser->verticalScrollBar()->setVisible(false);
    ui->infinite_line->setChecked(true);
    ui->traceText->viewport()->setAutoFillBackground(false);
}

void TraceWindow::setStyle(QString newStyleSheet)
{
    setStyleSheet(newStyleSheet);
}

void TraceWindow::wheelEvent(QWheelEvent* event)
{
    //При автоскролле работает немного неправильно, надо будет переделать, пока не критично
    QPoint numDegrees = event->angleDelta() / 8*(-1);

    /*    if(ui->Autoscroll->isChecked()){
        ui->Autoscroll->setChecked(false);
        reloadTracesFromBelow(ui->verticalScrollBar->value()-numberOfRowsToShow);
        std::cout<<ui->verticalScrollBar->value()<<std::endl;
    }
    else */if(numDegrees.ry()<0){
        sliderAction = 2;
        ui->Autoscroll->setChecked(false);
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()-1);
    }
    else if(numDegrees.ry()>0){
        ui->Autoscroll->setChecked(false);
        ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()+1);
    }

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
    QString color= "color:#C0C0C0\">";

    switch(g.bLevel){
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

    QString sequenceToGUI = QString::number(g.sequence);
    QString traceToGUI = g.trace;
    QString timeToGUI = "";
    QString traceToRightPanel = g.trace;

    if(argsThatNeedToBeChangedByEnum.contains(g.wID)){
        QList<ArgsThatNeedToBeChangedByEnum> args = argsThatNeedToBeChangedByEnum.value(g.wID);
        for(int i =args.size()-1;i>=0;i--){

            //0 - ничего ставить не надо
            tUINT32 number = traceToGUI.mid(g.argsPosition.value(i).argStart,(g.argsPosition.value(i).argEnd-g.argsPosition.value(i).argStart)).toInt();
            if(argsThatNeedToBeChangedByEnum.value(g.wID).at(i).enumId==0){
                continue;
            }

            if(!traceSettings->enumsIdList.contains(argsThatNeedToBeChangedByEnum.value(g.wID).at(i).enumId)){
                continue;
            }

            //на случай если айдишника енама нет в списке енамов то скипаем
            if(!traceSettings->getEnumParser()->enums.at(args.at(i).enumId-1).enums.contains(number)){
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

            //если все проверки прошли, но нам не надо показывать в главном экране измененный енам, но надо справа, делаем тут правку
            //костыль, попозже поправить
            if(argsThatNeedToBeChangedByEnum.value(g.wID).at(i).needToShow==Qt::Unchecked){
                traceToRightPanel.replace(g.argsPosition.value(i).argStart,(g.argsPosition.value(i).argEnd-g.argsPosition.value(i).argStart),traceSettings->getEnumParser()->enums.at(args.at(i).enumId-1).enums.value(number).name);
                continue;
            }

            traceToRightPanel.replace(g.argsPosition.value(i).argStart,(g.argsPosition.value(i).argEnd-g.argsPosition.value(i).argStart),traceSettings->getEnumParser()->enums.at(args.at(i).enumId-1).enums.value(number).name);
            traceToGUI.replace(g.argsPosition.value(i).argStart,(g.argsPosition.value(i).argEnd-g.argsPosition.value(i).argStart),
                               boldEnumStart+italicEnumStart+traceSettings->getEnumParser()->enums.at(args.at(i).enumId-1).enums.value(number).name+italicEnumEnd+boldEnumEnd);
        }
    }

    QString formattedWithEnumGUI = traceToGUI;

    QString sequenceHref = sequenceToGUI+" _splt_ "+traceToRightPanel+" _splt_ "+QString::number(g.positionInMap);
    sequenceHref = sequenceHref.trimmed();

    traceToGUI.insert(0," ");

    if(traceSettings->isSequenceColumnNeedToShow()==Qt::Unchecked){
        sequenceToGUI = "";
    }

    if(traceSettings->isTimeColumnNeedToShow()==Qt::Checked){
        QString seconds = QString::number(g.time.dwSeconds);
        if(seconds.length()<2){
            seconds.insert(0,"0");
        }

        QString minutes = QString::number(g.time.dwMinutes);
        if(minutes.length()<2){
            minutes.insert(0,"0");
        }

        QString hour = QString::number(g.time.dwHour);
        if(hour.length()<2){
            hour.insert(0,"0");
        }
        timeToGUI = " " + hour+":"+minutes+":"+seconds+" ";
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

void TraceWindow::closeEvent (QCloseEvent *event)
{
    traceSettings->close();
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
    QString fileName;
    if(guiData.size()>1){
        p7Time time = guiData.value(1).time;
        fileName = clientName.ip+"."+clientName.port+"-"+QString::number(time.dwHour)+"."+QString::number(time.dwMinutes)+"."+QString::number(time.dwSeconds);
    }
    else{
        fileName = clientName.ip+"."+clientName.port;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Save As",fileName,tr("Text files(*.txt"));
    if(filePath==""){
        return;
    }

    TracesToText* traces = new TracesToText(new QMap(guiData),filePath,this);
    traces->start();
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
        delete fileReadingGif;
        ui->fileReadingStatus->setPixmap(QPixmap(":/tick.png"));
        ui->fileReadingStatus->setScaledContents(true);
        ui->actionsStatusLabel->setText("File has been read");
    }else{
        fileReadingGif = new QMovie(":/loading.gif");
        ui->connectionStatus->setIcon(QIcon());
        ui->fileReadingStatus->setMovie(fileReadingGif);
        fileReadingGif->start();
        ui->actionsStatusLabel->setText("Reading file...");
    }
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

void TraceWindow::on_pushButton_clicked()
{
    clearSelect();
}


void TraceWindow::on_verticalScrollBar_actionTriggered(int action)
{
    ui->selectedLabel->clear();
    lastSelected=-1;
    std::cout<<"action value "<<ui->verticalScrollBar->value()<<std::endl;
    sliderAction = action;

}

