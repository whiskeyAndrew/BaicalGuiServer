#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H
#include <QSettings>
#include <QColor>
#include "GTypes.h"
class ConfigHandler
{

private:
    QSettings *settings;

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


    Qt::CheckState traceLevel = Qt::Checked;
    Qt::CheckState debugLevel = Qt::Checked;
    Qt::CheckState infoLevel = Qt::Checked;
    Qt::CheckState warningLevel = Qt::Checked;
    Qt::CheckState errorLevel = Qt::Checked;
    Qt::CheckState criticalLevel = Qt::Checked;
    ConfigHandler(QString connectionName);
    void SaveColors();
    void LoadColors();
    void SaveTraceLevelsToShow();
    void LoadTraceLevelsToShow();
};

#endif // CONFIGHANDLER_H
