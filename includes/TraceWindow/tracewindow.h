#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#include "../Trace.h"
#include <QWidget>
#include <queue>
#include "qcheckbox.h"
#include <QWheelEvent>
#include <QTextCursor>
#include <QDialog>
#include "tracewindowsettings.h"
#include <QThread>
#include "../tracestotxt.h"
#include <QIcon>

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
class TraceWindow : public QDialog
{
    Q_OBJECT

public:
    QList<tUINT32> isNeedToShowByTraceLevel;
    explicit TraceWindow(ConnectionName newClientName,ConfigHandler *newConfig, QDialog *parent = nullptr);
    ~TraceWindow();

    void setStyle(QString newStyleSheet);
    void setClientName(const QString &newClientName);

    QColor getTraceColor();
    void setTraceColor (QColor newTraceColor);
    QColor getDebugColor();
    void setDebugColor(QColor newDebugColor);
    QColor getInfoColor();
    void setInfoColor(QColor newInfoColor);
    QColor getWarningColor();
    void setWarningColor(QColor newWarningColor);
    QColor getErrorColor();
    void setErrorColor(QColor newErrorColor);
    QColor getCriticalColor();
    void setCriticalColor(QColor newCriticalColor);

    QString getTransparency() const;
    void setTransparency(QString newTransparency);

    void changeTraceLevelIsShownElement(tUINT32 id, tUINT32 state);
    const ConnectionName &getClientName() const;

    Qt::CheckState isAutoscrollChecked();
    tBOOL isInitialized() const;

    void SetActionStatusText(QString text);
    void recountNumberOfRowsToShow();
private:
    tUINT32 numberOfRowsToShow;
    QCheckBox autoscroll;
    TraceWindowSettings *traceSettings;
    ConfigHandler *config;

    //"style=\"background-color:rgba(255, 0, 0, 0.4)\""


    QColor traceColor = "";
    QColor debugColor = "";
    QColor infoColor = "";
    QColor warningColor = "";
    QColor errorColor = "";
    QColor criticalColor = "";
    QString transparency = "0.2";
    QString traceLinkStart = "<a ";
    QString traceLinkHref = "href=\"";
    //QString traceLinkMiddle = "\"color:#C0C0C0; text-decoration:none\">";
    QString traceLinkMiddle = "\"style=\"color:#C0C0C0;text-decoration:none;\">";
    QString traceLinkEnd = "</a>";

    QString traceText;
    QString sequence;

    QTextCursor cursor;

    tBOOL initEnded = false;

    //Для дозаполнения данных если не хватает строк и для плавного перехода
    tBOOL needToAppendFromBottom = false;
    tUINT32 tempCounterToRemember;

    QCheckBox *infiniteLine;

    QMap<tUINT32, GUIData> guiData;

    tUINT32 verticalBarSize = 0; //==guiData.size

    Trace *traceThread;
    Ui::TraceWindow *ui;
    QMap<int,QString> bLevels = {{0,"TRACE"},
                                 {1,"DEBUG"},
                                 {2,"INFO"},
                                 {3,"WARNING"},
                                 {4,"ERROR"},
                                 {5,"CRITICAL"}};


    ConnectionName clientName;
    QString styleSheet;

    void mousePressEvent(QMouseEvent *eventPress);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *e);
    void InitWindow();

    bool eventFilter(QObject *object, QEvent *event);

    tUINT32 firstInitCounter = 0;
    QString GetGuiRow(GUIData g);
    bool event(QEvent *event);
    void ReloadTracesFromBelow(int value);
    void ReloadTracesFromAbove(int value);

public slots:
    void GetTrace(TraceToGUI trace);
    void SetTraceAsObject(Trace *trace);
    void GetTraceFromFile(std::queue<TraceToGUI>);
    void traceRowListCheckboxChanged(tUINT32 wID,tUINT32 state);
    void AddUniqueTrace(UniqueTraceData trace);

    void ReloadTracesInsideWindow();
private slots:
    void AutoscrollStateChanged(tUINT32 stat);
    void on_expandButton_clicked(bool checked);
    void on_verticalScrollBar_valueChanged(int value);
    void on_Disable_clicked();
    void on_infinite_line_stateChanged(int arg1);
    void OffAutoscroll();
    void VerticalSliderReleased();
    void OpenHyperlink(const QUrl &link);
    void on_WindowSettings_clicked();
    void on_traceToTxt_clicked();
    void on_actionsStatusLabel_clicked();
};


#endif // TRACEWINDOW_H
