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
#include "../Trace.h"

struct ConnectionName{
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
    void uniqueTracesItemChanged(QListWidgetItem*  item);
    void modulesItemChanged(QListWidgetItem *item);
    void appendUniqueTracesList(QString text, tUINT32 wID);
    QMap<tUINT32,tUINT32> needToShowTraceByID;
    QMap<tUINT32,tUINT32> needToShowModules;

    void disableElement(tUINT32 wID);
    //~Unique Traces List

    Qt::CheckState isTraceColumnNeedToShow();
    Qt::CheckState isSequenceColumnNeedToShow();
    QCheckBox* getAutoTracesCount() const;

    QString getRowsOnScreen();

    void setTraceWindowSizeText();
    EnumParser* getEnumParser() const;
    QList<tUINT32> enumsIdList;

    Qt::CheckState isTimeColumnNeedToShow();
    bool isTraceBold();
    bool isDebugBold();
    bool isInfoBold();
    bool isWarningBold();
    bool isErrorBold();
    bool isCriticalBold();
    bool isTraceItalic();
    bool isDebugItalic();
    bool isInfoItalic();
    bool isWarningItalic();
    bool isErrorItalic();
    bool isCriticalItalic();
    void appendModules(sP7Trace_Module text);
    Qt::CheckState getIsEnumBold();

    Qt::CheckState getIsEnumItalic();

    tUINT32 getWheelScrollStep();

public slots:

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

    void on_clearEnum_clicked();

    void on_traceRowBold_clicked();

    void on_traceRowItalic_clicked();

    void on_debugRowBold_clicked();

    void on_debugRowItalic_clicked();

    void on_infoRowBold_clicked();

    void on_infoRowItalic_clicked();

    void on_warningRowBold_clicked();

    void on_warningRowItalic_clicked();

    void on_errorRowBold_clicked();

    void on_errorRowItalic_clicked();

    void on_criticalRowBold_clicked();

    void on_criticalRowItalic_clicked();

    void on_timeCheckbox_stateChanged(int arg1);

    void on_checkAllModules_clicked();

    void on_uncheckAllModules_clicked();

    void on_enumItalicCheckbox_stateChanged(int arg1);

    void on_enumBoldCheckbox_stateChanged(int arg1);

    void on_loadTracesToShowByIdFromConfig_clicked();

    void on_saveTracesToShowByIdToConfig_clicked();

    void on_loadModulesToShowFromConfig_clicked();

    void on_saveModulesToShowToConfig_clicked();

    void on_loadTypesFromConfig_clicked();

    void on_saveTypesToConfig_clicked();

    void on_wheelStepLineEdit_editingFinished();

private:
    Qt::CheckState isEnumBold = Qt::Unchecked;
    Qt::CheckState isEnumItalic = Qt::Unchecked;

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

    tUINT32 wheelScrollStep;

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
    void loadTracesToShowByIdFromConfig();
    void loadModulesToShowFromConfig();
    void loadTypesFromConfig();
signals:
    //Unique Traces List
    void reloadDataInsideTraceWindow();
};

#endif // TRACEWINDOWSETTINGS_H
