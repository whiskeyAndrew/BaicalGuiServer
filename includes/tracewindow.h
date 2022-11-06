#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#include "Trace.h"
#include <QWidget>
#include <queue>
#include "qcheckbox.h"
#include "tracerowslist.h"
#include <QWheelEvent>
#include <QTextCursor>

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

struct GUIData{
    tUINT32 sequence;
    QString trace;
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
    void setClientName(const QString &newClientName);

private:
    time_t autoscrollTime = 0;
    QString traceLinkStart = "<a href=\"";
    QString traceLinkMiddle = "\"style=\"color:#C0C0C0\"style=\"text-decoration:none\">";
    QString traceLinkEnd = "</a>";

    QString traceText;
    QString sequence;

    QTextCursor cursor;

    QCheckBox *infiniteLine;

    QMap<tUINT32, GUIData> guiData;

    Trace *traceThread;
    Ui::TraceWindow *ui;
    QMap<int,QString> bLevels = {{0,"TRACE"},
                                 {1,"DEBUG"},
                                 {2,"INFO"},
                                 {3,"WARNING"},
                                 {4,"ERROR"},
                                 {5,"CRITICAL"}};


    QString clientName;
    QString styleSheet;

    void mousePressEvent(QMouseEvent *eventPress);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void InitWindow();

    bool eventFilter(QObject *object, QEvent *event);
    TraceRowsList *rawTraces;

    tUINT32 firstInitCounter = 0;
public slots:
    void GetTrace(TraceToGUI trace);
    void SetTraceAsObject(Trace *trace);
    void GetTraceFromFile(std::queue<TraceToGUI>);
    void traceRowListCheckboxChanged(tUINT32 wID,tUINT32 state);


private slots:
    void AutoscrollStateChanged(tUINT32 stat);
    void on_expandButton_clicked(bool checked);
    void on_pushButton_clicked();
    void on_verticalScrollBar_valueChanged(int value);
    void on_Disable_clicked();
    void on_infinite_line_stateChanged(int arg1);
    void on_UniqueRows_clicked();
    void OffAutoscroll();
    void VerticalSliderReleased();
    void OpenHyperlink(const QUrl &link);
};

#endif // TRACEWINDOW_H
