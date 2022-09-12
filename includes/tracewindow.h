#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#include "Trace.h"
#include <QWidget>

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
    Ui::TraceWindow *ui;
public slots:
    void GetTrace(TraceLineData trace);
    void GetQueueSize(tUINT32 size);
};

#endif // TRACEWINDOW_H
