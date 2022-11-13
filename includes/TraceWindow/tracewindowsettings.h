#ifndef TRACEWINDOWSETTINGS_H
#define TRACEWINDOWSETTINGS_H
#include <QWidget>
#include <QColorDialog>
#include "../Trace.h"
#include <QListWidgetItem>
#include <QMessageBox>

class TraceWindow;
namespace Ui {
class TraceWindowSettings;
}

class TraceWindowSettings : public QWidget
{
    Q_OBJECT

public:    
    TraceWindowSettings(TraceWindow *newTraceWindow);
    ~TraceWindowSettings();

    void createConnections();

    //Unique Traces List
    void itemChanged(QListWidgetItem * item);
    void AppendUniqueTracesList(QString text, tUINT32 wID);
    QMap<tUINT32,tUINT32> needToShow;
    void DisableElement(tUINT32 wID);
    //~Unique Traces List

    void SetWindowName(QString name);
    tUINT32 isTraceColumnNeedToShow();
    tUINT32 isSequenceColumnNeedToShow();
private slots:
    //Trace Colors
    void on_traceColorButton_clicked();
    void on_debugColorButton_clicked();
    void on_infoColorButton_clicked();
    void on_warningColorButton_clicked();
    void on_errorColorButton_clicked();
    void on_criticalColorButton_clicked();

    void on_clearTrace_clicked();
    void on_clearDebug_clicked();
    void on_clearInfo_clicked();
    void on_clearWarning_clicked();
    void on_clearError_clicked();
    void on_clearCritical_clicked();

    void on_lineEdit_editingFinished();

    void on_traceCheckBox_stateChanged(int arg1);

    void on_debugCheckBox_stateChanged(int arg1);

    void on_infoCheckBox_stateChanged(int arg1);

    void on_warningCheckBox_stateChanged(int arg1);

    void on_errorCheckBox_stateChanged(int arg1);

    void on_criticalCheckBox_stateChanged(int arg1);

private:
    //TraceColors
    QColorDialog *colorDialog;
    QColor traceColor;
    QColor debugColor;
    QColor infoColor;
    QColor warningColor;
    QColor errorColor;
    QColor criticalColor;

    TraceWindow *traceWindow;

    Ui::TraceWindowSettings *ui;


    void ReloadColors();
    void InitColors();
signals:
    //Unique Traces List
    void SendRowWID(tUINT32 wID, tUINT32 state);
};

#endif // TRACEWINDOWSETTINGS_H
