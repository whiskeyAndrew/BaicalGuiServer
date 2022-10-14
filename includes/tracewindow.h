#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#include "Trace.h"
#include <QWidget>
#include <QTableWidgetItem>
#include <queue>

#include <QItemDelegate>
#include <QPainter>

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


class Delegate : public QItemDelegate
{
    Q_OBJECT
public:
    Delegate(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class TraceViewer : public QAbstractTableModel
{
    Q_OBJECT

public:
    TraceViewer(QObject *parent = 0);

    void populateData(int scrollValue);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void initTable();
    void fillTempList(QString text, QString sequence, QString timer);


    const QList<QString> &getTempText() const;

private:
    QList<QString> tempText;
    QList<QString> tempSequence;
    QList<QString> tempTimer;

    QList<QString> traceText;
    QList<QString> traceSequence;
    QList<QString> traceTimer;

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

    void setStyle(QString newStyleSheet);

private:
    QList<QString> traceList;
    QList<QString> sequenceList;
    TraceViewer *traceViewer;
    Trace *traceThread;
    Ui::TraceWindow *ui;
    QMap<int,QString> bLevels = {{0,"TRACE"},
                                 {1,"DEBUG"},
                                 {2,"INFO"},
                                 {3,"WARNING"},
                                 {4,"ERROR"},
                                 {5,"CRITICAL"}};
    void resizeEvent(QResizeEvent *e);
    void InitWindow();

    QString styleSheet;
//    void wheelEvent(QWheelEvent *event);
public slots:
    void GetTrace(TraceToGUI trace);
    void SetTraceAsObject(Trace *trace);
    void GetTraceFromFile(std::queue<TraceToGUI>);

    void onTableClicked(const QModelIndex &index);

private slots:
    void on_expandButton_clicked(bool checked);
    void on_pushButton_clicked();
    void on_column0_stateChanged(int arg1);
    void on_Time_stateChanged(int arg1);
    void on_verticalScrollBar_valueChanged(int value);
    void on_expandButton_clicked();
};

#endif // TRACEWINDOW_H
