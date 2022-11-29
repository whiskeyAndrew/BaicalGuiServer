#ifndef TRACEWINDOWSETTINGS_H
#define TRACEWINDOWSETTINGS_H
#include <QWidget>
#include <QColorDialog>
#include "../Trace.h"
#include <QListWidgetItem>
<<<<<<< Updated upstream
=======
#include <QMessageBox>
#include "../confighandler.h"
#include <QTextStream>
#include <QFileDialog>

struct ConnectionName{
    QString status;
    QString ip;
    QString port;
};
>>>>>>> Stashed changes

class TraceWindow;
namespace Ui {
class TraceWindowSettings;
}

class TraceWindowSettings : public QWidget
{
    Q_OBJECT

public:    
<<<<<<< Updated upstream
    TraceWindowSettings(TraceWindow *newTraceWindow);
=======
    TraceWindowSettings(TraceWindow *newTraceWindow,ConnectionName *clientName, ConfigHandler *newConfig);
>>>>>>> Stashed changes
    ~TraceWindowSettings();

    void createConnections();

    //Unique Traces List
    void itemChanged(QListWidgetItem * item);
    void AppendUniqueTracesList(QString text, tUINT32 wID);
    QMap<tUINT32,tUINT32> needToShow;
    void DisableElement(tUINT32 wID);
    //~Unique Traces List

<<<<<<< Updated upstream
=======
    void SetWindowName(QString name);
    Qt::CheckState isTraceColumnNeedToShow();
    Qt::CheckState isSequenceColumnNeedToShow();
    QMessageBox mbx;

>>>>>>> Stashed changes
private slots:
    //Trace Colors
    void on_traceColorButton_clicked();
    void on_debugColorButton_clicked();
    void on_infoColorButton_clicked();
    void on_warningColorButton_clicked();
    void on_errorColorButton_clicked();
    void on_criticalColorButton_clicked();

<<<<<<< Updated upstream
=======
    void on_clearTrace_clicked();
    void on_clearDebug_clicked();
    void on_clearInfo_clicked();
    void on_clearWarning_clicked();
    void on_clearError_clicked();
    void on_clearCritical_clicked();

    void on_debugCheckBox_stateChanged(int arg1);
    void on_infoCheckBox_stateChanged(int arg1);
    void on_warningCheckBox_stateChanged(int arg1);
    void on_errorCheckBox_stateChanged(int arg1);
    void on_criticalCheckBox_stateChanged(int arg1);
    void on_traceLevelCheckBox_stateChanged(int arg1);

    void on_sequenceCheckbox_stateChanged(int arg1);

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_sliderMoved(int position);

    void on_checkAllUniqueTraces_clicked();

    void on_uncheckAllUniqueTraces_clicked();

    void on_traceCheckbox_stateChanged(int arg1);

    void on_tabWidget_tabBarClicked(int index);

    void on_saveButton_clicked();

    void on_LoadDataFromConfig_clicked();

    void on_saveAsButton_clicked();

    void on_loadButton_clicked();

>>>>>>> Stashed changes
private:
    QColorDialog *colorDialog;
    QColor rgbFromColorDialog;

    TraceWindow *traceWindow;

    Ui::TraceWindowSettings *ui;

<<<<<<< Updated upstream
=======

    void ReloadColors();
    void InitColors();
    void InitTraceLevels();
    void LoadConfigFileAsText();
>>>>>>> Stashed changes
signals:
    //Unique Traces List
    void SendRowWID(tUINT32 wID, tUINT32 state);
};

#endif // TRACEWINDOWSETTINGS_H
