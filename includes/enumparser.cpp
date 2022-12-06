#include "enumparser.h"

EnumParser::EnumParser(QString fileName)
{
    file = new QFile(fileName);
}

void EnumParser::readEnumsFromFile(){
    if (file->open(QIODevice::ReadOnly))
    {
        QString line;
        QString subLine;
        QString enumName;
        QTextStream in(file);
        while (!in.atEnd())
        {
            enumName = line;
            line = in.readLine();
            if(line.contains("{")){
                if(enumName == "" || enumName.contains("};")){
                    enumName = line;
                }
                enumName = enumName.replace("{","");
                enumName = enumName.replace("enum","");
                enumName = enumName.trimmed();
                tUINT32 counter = 0;
                QMap<tUINT32, enumFromFile> tempEnumsMap;
                //нашли первую группу
                while(!in.atEnd()){
                    line = in.readLine();
                    if(line.contains("}")){
                        break;
                    }

                    if(line.contains("=")){
                        subLine = line.mid(0,line.indexOf("="));
                        QRegExp rx("= \\d+");
                        rx.indexIn(line);
                        QStringList list = rx.capturedTexts();
                        tUINT32 number = list.at(0).toInt();
                        counter = number;
                        tempEnumsMap.insert(number,{subLine,""});
                    } else{
                        line = line.replace(",","");
                        tempEnumsMap.insert(counter,{line,""});
                        counter++;
                    }
                }
                enums.insert(enumName,tempEnumsMap);
            }
        }
    }
    file->close();
}

