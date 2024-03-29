#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H

#define TRACES_UNTIL_RELOAD_PAGE 500

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
#include <QRgb>
#include <QCloseEvent>
#include <QIcon>
#include <QMovie>
#include <QDebug>

struct rowsInsideTrace{
    tUINT32 traceNumber;
    tUINT32 lineNumber;
};

enum Theme{
    DEFAULT = 0,
    DARK
};

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

class MainWindow;
class FileReader;
namespace Ui {
class TraceWindow;
}
class TraceWindow : public QDialog
{
    Q_OBJECT

public:
    QList<tUINT32> isNeedToShowByTraceLevel;
    explicit TraceWindow(ConnectionName newClientName,ConfigHandler* newConfig, MainWindow* mw, QDialog* parent = nullptr);
    ~TraceWindow();


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

    void setActionStatusText(QString text);
    void recountNumberOfRowsToShow();

    Trace* traceThread;
    TraceWindowSettings* getTraceSettings() const;
    void appendArgsThatNeedToBeChangedByEnum(tUINT32 wID, QList<ArgsThatNeedToBeChangedByEnum> args);

    void clearArgsThatNeedToBeChangedByEnumm();
    QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > getArgsThatNeedToBeChangedByEnum();

    void setArgsThatNeedToBeChangedByEnum(QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > newArgsThatNeedToBeChangedByEnum);

    QColor getEmptyColor();

    void setConnectionStatus(tUINT32 status);
    void clearOneEnumElement(tUINT32 wID);
    void fileReadingStatus(tUINT32 percent);
    tUINT32 getConnectionStatus();
    MainWindow *getMainWindow() const;

    void setAutoscrollDisabled(bool status);
    bool isRowNeedToBeShown(GUIData g);
    void recountNubmerOfTracesToShow();
    FileReader *getFileReader() const;
    void setFileReader(FileReader *newFileReader);

    void changeTheme(Theme theme);
private:
    tUINT32 tracesToReloadCounter = 0;
    FileReader* fileReader = NULL;
    tINT32 fileHasBeenRead = -1; // -1 = not exists, 0 - reading, 1 - read
    QMovie* loadingGif;
    tUINT32 sequenceToRememberForReloadingAtProperPlace = 0;
    MainWindow* mainWindow;
    tUINT32 connectionStatus;
    tUINT32 numberOfRowsToShow;
    QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum>> argsThatNeedToBeChangedByEnum;
    QCheckBox autoscroll;
    TraceWindowSettings* traceSettings;
    ConfigHandler* config;

    //в инициализации устанавливает значение 0 0 0 255, стандартное значение если инициализация цвета из конфига не прошла
    QColor emptyColor;
    QColor traceColor = "";
    QColor debugColor = "";
    QColor infoColor = "";
    QColor warningColor = "";
    QColor errorColor = "";
    QColor criticalColor = "";
    QString transparency = "0.2";
    QString traceLinkStart = "<a ";

    //"style=\"background-color:rgba(255, 0, 0, 0.4)\""
    QString traceLinkHref = "href=\"";
    QString traceLinkMiddle = "\"style=\"text-decoration:none;"; //color:#C0C0C0;
    QString traceLinkEnd = "</a>";

    QString traceText;
    QString sequence;

    QTextCursor cursor;

    tBOOL initEnded = false;
    tINT32 lastSelected = -1;
    //Для дозаполнения данных если не хватает строк и для плавного перехода
    tBOOL needToAppendFromBottom = false;
    tUINT32 tempCounterToRemember;

    tUINT32 lastScrollValue = -1;

    QCheckBox* infiniteLine;

    QList<GUIData>* guiData;
    QList<rowsInsideTrace> listOfRowsThatWeNeedToShow;

    Ui::TraceWindow* ui;
    QMap<int,QString> bLevels = {{0,"TRACE"},
                                 {1,"DEBUG"},
                                 {2,"INFO"},
                                 {3,"WARNING"},
                                 {4,"ERROR"},
                                 {5,"CRITICAL"}};

    ConnectionName clientName;
    QString styleSheet;

    void mousePressEvent(QMouseEvent* eventPress);
    void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent* e);
    void initWindow();

    bool eventFilter(QObject* object, QEvent* event);

    tUINT32 lastRenderedRow = -1;

    QString getGuiRow(GUIData* g);
//    bool event(QEvent* event);
    void reloadTracesFromBelow(int value);
    void reloadTracesFromAbove(int value);

    void closeEvent(QCloseEvent *event);
    void clearSelect();
    void deleteFirstLineInsideTracesWindow();
public slots:
    void setSettingsDisabled(bool status);
    void getTrace(GUIData uniqueTrace);
    void setTraceAsObject(Trace* trace);
    void traceRowListCheckboxChanged();
    void addUniqueTrace(UniqueTraceData trace);

    void reloadTracesInsideWindow();
    void addModule(sP7Trace_Module module);
private slots:
    void autoscrollStateChanged(tUINT32 stat);
    void on_expandButton_clicked(bool checked);
    void on_verticalScrollBar_valueChanged(int value);
    void on_Disable_clicked();
    void on_infinite_line_stateChanged(int arg1);
    void offAutoscroll();
    void verticalSliderReleased();
    void openHyperlink(const QUrl &link);
    void on_WindowSettings_clicked();
    void on_traceToTxt_clicked();
    void on_actionsStatusLabel_clicked();
    void on_hideServerStatus_clicked();
    void on_verticalScrollBar_sliderPressed();
    void on_clearSelected_clicked();
    void on_verticalScrollBar_actionTriggered(int action);
};

#endif // TRACEWINDOW_H
