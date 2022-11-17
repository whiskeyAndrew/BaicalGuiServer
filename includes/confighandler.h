#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H
#include <QSettings>
#include <QColor>

class ConfigHandler
{

private:
    QSettings *settings;

    //TraceSettings

public:
    QColor traceColor;
    QColor debugColor;
    QColor infoColor;
    QColor warningColor;
    QColor errorColor;
    QColor criticalColor;

    ConfigHandler();
    void Save();
    void LoadColors();
};

#endif // CONFIGHANDLER_H
