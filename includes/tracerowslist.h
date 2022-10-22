#ifndef TRACEROWSLIST_H
#define TRACEROWSLIST_H

#include "includes/GTypes.h"
#include <QWidget>
#include <QListWidget>
#include <iostream>
class TraceWindow;

namespace Ui {
class TraceRowsList;
}

class TraceRowsList : public QWidget
{
    Q_OBJECT

public:
    TraceWindow *traceWindow;
    TraceRowsList(TraceWindow *newTraceWindow, QWidget *parent = nullptr){
        traceWindow = newTraceWindow;
    }
    explicit TraceRowsList(QWidget *parent = nullptr);
    ~TraceRowsList();
    void AppendList(QString text, tUINT32 needToShow,tUINT32 wID);

    void itemChanged(QListWidgetItem * item);

    void createConnections();
    TraceWindow *getTraceWindow() const;
    void setTraceWindow(TraceWindow *newTraceWindow);

private:
    Ui::TraceRowsList *ui;
signals:
    void SendRowWID(tUINT32 wID, tUINT32 state);
};

#endif // TRACEROWSLIST_H
