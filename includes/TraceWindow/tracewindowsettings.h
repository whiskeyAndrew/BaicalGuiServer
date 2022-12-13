#ifndef TRACEWINDOWSETTINGS_H
#define TRACEWINDOWSETTINGS_H
#include <QWidget>
#include <QColorDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include "../confighandler.h"
#include <QTextStream>
#include <QFileDialog>
#include <QDateTime>
#include <QCheckBox>
#include "../enumparser.h"
#include <QComboBox>

struct ConnectionName{
    QString status;
    QString ip;
    QString port;
};

class TraceWindow;
namespace Ui {
class TraceWindowSettings;
}

class TraceWindowSettings : public QWidget
{
    Q_OBJECT

public:
    TraceWindowSettings(TraceWindow* newTraceWindow,ConnectionName* clientName);
    ~TraceWindowSettings();


    //Unique Traces List
    void itemChanged(QListWidgetItem*  item);
    void appendUniqueTracesList(QString text, tUINT32 wID);
    QMap<tUINT32,tUINT32> needToShow;
    void disableElement(tUINT32 wID);
    //~Unique Traces List

    Qt::CheckState isTraceColumnNeedToShow();
    Qt::CheckState isSequenceColumnNeedToShow();
    QCheckBox* getAutoTracesCount() const;

    QString getRowsOnScreen();

    void setTraceWindowSizeText();
    EnumParser* getEnumParser() const;
    QList<tUINT32> enumsIdList;

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



    void on_saveAllTraceCheckboxes_clicked();

    void on_saveAllColors_clicked();

    void on_autoRowsCounter_stateChanged(int arg1);

    void on_rowsOnScreen_editingFinished();

    void on_saveWindowsProperties_clicked();

    void on_loadEnumsFromTXT_clicked();

    void on_enumsList_itemClicked(QListWidgetItem* item);

    void on_applyEnumToTraceById_clicked();

    void on_traceIDforEnums_currentIndexChanged(int index);

    void on_saveAllSettings_clicked();

    void on_saveEnumsToConfig_clicked();

    void on_clearEnums_clicked();

    void on_loadEnumsFromConfig_clicked();

private:
    ConnectionName connectionName;
    ConfigHandler* config;
    EnumParser* enumParser;
    //TraceColors
    QColor color;
    QColorDialog* colorDialog;

    QList<QComboBox*> comboBoxesToDelete;
    QString rowsOnScreen;
    QCheckBox* autoTracesCount;

    TraceWindow* traceWindow;

    Ui::TraceWindowSettings* ui;

    void reloadColors();
    void initColors();
    void initTraceLevels();
    void loadConfigFileAsText();
    void initWindowsSize();
    void resizeEvent(QResizeEvent* e);
    void reloadListOfArgsAndEnums();
    void loadEnumsFromConfig();
    void initWindow();

    tBOOL loadEnumsFromFile(QString fileName);
signals:
    //Unique Traces List
    void sendRowWID(tUINT32 wID, tUINT32 state);
};

#endif // TRACEWINDOWSETTINGS_H
