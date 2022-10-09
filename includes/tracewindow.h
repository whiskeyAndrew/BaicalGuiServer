#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#include "Trace.h"
#include <QWidget>
#include <QTableWidgetItem>
#include <queue>

enum eP7Trace_Level
{
    EP7TRACE_LEVEL_TRACE        = 0,
    EP7TRACE_LEVEL_DEBUG           ,
    EP7TRACE_LEVEL_INFO            ,
    EP7TRACE_LEVEL_WARNING         ,
    EP7TRACE_LEVEL_ERROR           ,
    EP7TRACE_LEVEL_CRITICAL        ,

    EP7TRACE_LEVEL_COUNT
};

class TraceViewer : public QAbstractTableModel
{
    Q_OBJECT

public:
    TraceViewer(QObject *parent = 0);

    void populateData(const QList<QString> &contactName,const QList<QString> &contactPhone);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<QString> traceText;
    QList<QString> traceSequence;

};

namespace Ui {
class TraceWindow;
}
class TraceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TraceWindow(QWidget *parent = nullptr);
    ~TraceWindow();

private:    
    Trace *traceThread;
    Ui::TraceWindow *ui;
    QMap<int,QString> bLevels = {{0,"TRACE"},
                                 {1,"DEBUG"},
                                 {2,"INFO"},
                                 {3,"WARNING"},
                                 {4,"ERROR"},
                                 {5,"CRITICAL"}};
public slots:
    void GetTrace(TraceToGUI trace);
    void GetQueueSize(tUINT32 size);
    void SetTraceAsObject(Trace *trace);
    void GetTraceFromFile(std::queue<TraceToGUI>);
private slots:
    void on_tableWidget_itemClicked(QTableWidgetItem *item);
    void on_expandButton_clicked(bool checked);
};

#endif // TRACEWINDOW_H
