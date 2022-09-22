#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#include "Trace.h"
#include <QWidget>
#include <QTableWidgetItem>

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
public slots:
    void GetTrace(TraceLineData trace);
    void GetQueueSize(tUINT32 size);
    void SetTraceAsObject(Trace *trace);
private slots:
    void on_tableWidget_itemClicked(QTableWidgetItem *item);
    void on_expandButton_clicked(bool checked);
};

#endif // TRACEWINDOW_H
