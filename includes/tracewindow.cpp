#include "ui_tracewindow.h"
#include "tracewindow.h"
#include <QDateTime>
#include <QStyledItemDelegate>


void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    QString txt = index.model()->data( index, Qt::DisplayRole ).toString();

    if(index.row() == 0)//green row
        painter->fillRect(option.rect,QColor(0,255,0));
    else
        if(index.row() == 1)//blue row
            painter->fillRect(option.rect,option.backgroundBrush);
        else
            if(index.row() == 2)//red row
                painter->fillRect(option.rect,QColor(255,0,0));
    //and so on

    if( option.state & QStyle::State_Selected )//we need this to show selection
    {
        painter->fillRect( option.rect, option.palette.highlight() );
    }
}

TraceWindow::TraceWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceWindow)
{
    this->setWindowFlags(Qt::Window);
    ui->setupUi(this);
    traceViewer = new TraceViewer(this);
    traceViewer->initTable();
    ui->tableView->setModel(traceViewer);
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->hide();

    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setFont(QFont("Courier",8));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    //Коннект по нажатию на ячейку
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
    //ui->tableView->setItemDelegate(new Delegate);
}

void TraceWindow::onTableClicked(const QModelIndex &index)
{
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


void TraceWindow::GetTrace(TraceToGUI trace)
{
    int countNumber = traceViewer->rowCount();
    sP7Trace_Data traceData = traceThread->GetTraceData(trace.sequence);

    traceViewer->populateData(QString::number(trace.sequence),trace.trace);
    ui->tableView->resizeRowToContents(countNumber);


    //Вот эта штука может вызывать пролаги при большом количестве данных
    //По логике вещей она обновляет ВСЮ таблицу при каждом инсерте, что делать, конечно, неправильно
    //Я в этом не уверен, но 100к данных обновляющихся выдерживает, надо оставить надолго и проверить потом как оно
    //Себя поведет

    emit traceViewer->layoutChanged();

}


void TraceWindow::GetTraceFromFile(std::queue<TraceToGUI> data){

    int counter = data.size();

    for(int i =0;i<counter;i++){
        TraceToGUI trace = data.front();
        data.pop();

        traceViewer->populateData(QString::number(trace.sequence),trace.trace);

    }
    emit traceViewer->layoutChanged();
    this->show();
    ui->tableView->resizeRowsToContents();
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

TraceViewer::TraceViewer(QObject *parent) : QAbstractTableModel(parent)
{
}

void TraceViewer::populateData(QString sequence, QString trace)
{
    traceSequence.append(sequence);
    traceText.append(trace);
}

int TraceViewer::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return traceSequence.length();
}

int TraceViewer::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant TraceViewer::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return traceSequence[index.row()];
    } else if (index.column() == 1) {
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
            return QString("Trace");
        }
    }
    return QVariant();
}

void TraceViewer::initTable()
{
    traceSequence.clear();
    traceText.clear();
}

