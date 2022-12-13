#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H
#include <QSettings>
#include <QColor>
#include "GTypes.h"

struct ArgsThatNeedToBeChangedByEnum{
    tUINT32 argId;
    tUINT32 enumId;
};
class ConfigHandler
{

private:
    //QSettings *settings;
    QString configFileName = "config.ini";

    //TraceSettings

public:
    QString configName;

    QColor traceColor;
    QColor debugColor;
    QColor infoColor;
    QColor warningColor;
    QColor errorColor;
    QColor criticalColor;
    QString transparency;

    tUINT32 traceWindow_x = 0;
    tUINT32 traceWindow_y = 0;
    tUINT32 traceSettingsWindow_x = 0;
    tUINT32 traceSettingsWindow_y = 0;

    Qt::CheckState traceLevel = Qt::Checked;
    Qt::CheckState debugLevel = Qt::Checked;
    Qt::CheckState infoLevel = Qt::Checked;
    Qt::CheckState warningLevel = Qt::Checked;
    Qt::CheckState errorLevel = Qt::Checked;
    Qt::CheckState criticalLevel = Qt::Checked;
    ConfigHandler(QString connectionName);
    ConfigHandler();
    void SaveColors();
    void LoadColors();
    void SaveTraceLevelsToShow();
    void LoadTraceLevelsToShow();
    const QString &getConfigFileName() const;
    void SaveWindowsSize(tUINT32 tx,tUINT32 ty, tUINT32 tsx, tUINT32 tsy);
    void LoadWindowsSize();
    void SaveEnumsList(QString ip, QString fileName);
    QString LoadEnumsList(QString ip);
    tUINT32 SaveEnums(QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > args, QString ip);
    QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > LoadEnums(QString ip);
};

#endif // CONFIGHANDLER_H
