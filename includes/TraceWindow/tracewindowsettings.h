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

private:
    //TraceColors
    QColorDialog *colorDialog;
    QColor traceColor;
    QColor debugColor;
    QColor infoColor;
    QColor warningColor;
    QColor errorColor;
    QColor criticalColor;

    QString transparency = "0.2";

    TraceWindow *traceWindow;

    Ui::TraceWindowSettings *ui;


    void ReloadColors();
signals:
    //Unique Traces List
    void SendRowWID(tUINT32 wID, tUINT32 state);
};

#endif // TRACEWINDOWSETTINGS_H
