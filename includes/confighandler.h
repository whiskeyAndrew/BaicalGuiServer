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
    QString configFileName = "config.ini";

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
    void saveColors();
    void loadColors();
    void saveTraceLevelsToShow();
    void loadTraceLevelsToShow();
    QString getConfigFileName();
    void saveWindowsSize(tUINT32 tx,tUINT32 ty, tUINT32 tsx, tUINT32 tsy);
    void loadWindowsSize();
    void saveEnumsList(QString ip, QString fileName);
    QString loadEnumsList(QString ip);
    tUINT32 saveEnums(QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > args, QString ip);
    QMap<tUINT32, QList<ArgsThatNeedToBeChangedByEnum> > loadEnums(QString ip);
};

#endif // CONFIGHANDLER_H
