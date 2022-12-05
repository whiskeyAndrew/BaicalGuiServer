#include "confighandler.h"

//На случай, если появится вопрос, зачем каждый раз пересоздавать сеттингс?
//При изменении конфиг.ини ручками приложение не подтянет данные оттуда, потому что QSettings обращается к данным внутри себя, а не к файлу, если уже был создан
//Создавая QSettings, он подгружает данные из конфига при старте, что позволяет курировать уже с данными из файла напрямую, а не с данными из объекта
//Странная тема, особенно то, что нет методов на форсированное чтение данных из файла
const QString &ConfigHandler::getConfigFileName() const
{
    return configFileName;
}

ConfigHandler::ConfigHandler(QString connectionName)
{
    configName = connectionName;

    //Надо разобраться как форсированно грузить данные из settings

    //    QSettings *settings = new QSettings(configName + "_config.ini", QSettings::IniFormat );
    //     delete settings;
}

ConfigHandler::ConfigHandler()
{
    //    QSettings *settings = new QSettings("config.ini", QSettings::IniFormat );
    //    delete settings;
}

void ConfigHandler::SaveColors(){
    QSettings *settings = new QSettings("config.ini", QSettings::IniFormat );
    settings->beginGroup("RowsColors");
    settings->setValue("trace",QString::number(traceColor.red())+" "+QString::number(traceColor.green())+" "+QString::number(traceColor.blue()));
    settings->setValue("debug",QString::number(debugColor.red())+" "+QString::number(debugColor.green())+" "+QString::number(debugColor.blue()));
    settings->setValue("info",QString::number(infoColor.red())+" "+QString::number(infoColor.green())+" "+QString::number(infoColor.blue()));
    settings->setValue("warning",QString::number(warningColor.red())+" "+QString::number(warningColor.green())+" "+QString::number(warningColor.blue()));
    settings->setValue("error",QString::number(errorColor.red())+" "+QString::number(errorColor.green())+" "+QString::number(errorColor.blue()));
    settings->setValue("critical",QString::number(criticalColor.red())+" "+QString::number(criticalColor.green())+" "+QString::number(criticalColor.blue()));
    settings->setValue("transparency",transparency);
    settings->endGroup();
    delete settings;
}

void ConfigHandler::LoadColors(){
    QSettings *settings = new QSettings("config.ini", QSettings::IniFormat );
    settings->beginGroup("RowsColors");
    QStringList trace = settings->value("trace","").toString().split(" ");
    QStringList debug = settings->value("debug","").toString().split(" ");
    QStringList info = settings->value("info","").toString().split(" ");
    QStringList warning = settings->value("warning","").toString().split(" ");
    QStringList error = settings->value("error","").toString().split(" ");
    QStringList critical = settings->value("critical","").toString().split(" ");
    transparency = settings->value("transparency","0").toString();
    settings->endGroup();

    if(trace.size()==3){
        traceColor.setRgb(trace.at(0).toInt(),trace.at(1).toInt(),trace.at(2).toInt());
    }
    if(debug.size()==3){
        debugColor.setRgb(debug.at(0).toInt(),debug.at(1).toInt(),debug.at(2).toInt());
    }
    if(info.size()==3){
        infoColor.setRgb(info.at(0).toInt(),info.at(1).toInt(),info.at(2).toInt());
    }
    if(warning.size()==3){
        warningColor.setRgb(warning.at(0).toInt(),warning.at(1).toInt(),warning.at(2).toInt());
    }
    if(error.size()==3){
        errorColor.setRgb(error.at(0).toInt(),error.at(1).toInt(),error.at(2).toInt());
    }
    if(critical.size()==3){
        criticalColor.setRgb(critical.at(0).toInt(),critical.at(1).toInt(),critical.at(2).toInt());
    }
    delete settings;
}

void ConfigHandler::SaveTraceLevelsToShow(){
    QSettings *settings = new QSettings("config.ini", QSettings::IniFormat );
    settings->beginGroup("TraceLevelsToShow");
    settings->setValue("trace",traceLevel);
    settings->setValue("debug",debugLevel);
    settings->setValue("info",infoLevel);
    settings->setValue("warning",warningLevel);
    settings->setValue("error",errorLevel);
    settings->setValue("critical",criticalLevel);
    settings->endGroup();
    delete settings;
}

void ConfigHandler::LoadTraceLevelsToShow(){
    QSettings *settings = new QSettings("config.ini", QSettings::IniFormat );
    settings->beginGroup("TraceLevelsToShow");
    traceLevel = static_cast<Qt::CheckState>(settings->value("trace",2).toInt());
    debugLevel = static_cast<Qt::CheckState>(settings->value("debug",2).toInt());
    infoLevel = static_cast<Qt::CheckState>(settings->value("info",2).toInt());
    warningLevel = static_cast<Qt::CheckState>(settings->value("warning",2).toInt());
    errorLevel = static_cast<Qt::CheckState>(settings->value("error",2).toInt());
    criticalLevel =  static_cast<Qt::CheckState>(settings->value("critical",2).toInt());
    settings->endGroup();
    delete settings;
}

void ConfigHandler::SaveWindowsSize(tUINT32 tx,tUINT32 ty, tUINT32 tsx, tUINT32 tsy){
    traceWindow_x = tx;
    traceWindow_y = ty;
    traceSettingsWindow_x = tsx;
    traceSettingsWindow_y = tsy;

    QSettings *settings = new QSettings("config.ini", QSettings::IniFormat );
    settings->beginGroup("TraceWindowsSize");
    settings->setValue("traceWindow_x",traceWindow_x);
    settings->setValue("traceWindow_y",traceWindow_y);
    settings->setValue("traceSettingsWindow_x",traceSettingsWindow_x);
    settings->setValue("traceSettingsWindow_y",traceSettingsWindow_y);

    settings->endGroup();
    delete settings;
}

void ConfigHandler::LoadWindowsSize(){
    QSettings *settings = new QSettings("config.ini", QSettings::IniFormat );
    settings->beginGroup("TraceWindowsSize");
    traceWindow_x = settings->value("traceWindow_x").toInt();
    traceWindow_y = settings->value("traceWindow_y").toInt();
    traceSettingsWindow_x = settings->value("traceSettingsWindow_x").toInt();
    traceSettingsWindow_y = settings->value("traceSettingsWindow_y").toInt();

    settings->endGroup();
    delete settings;
}
