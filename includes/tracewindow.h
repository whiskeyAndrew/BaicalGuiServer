#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#include "Trace.h"
#include <QWidget>
#include <QTableWidgetItem>
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
