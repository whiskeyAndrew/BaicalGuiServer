#include "enumparser.h"

EnumParser::EnumParser()
{

}

tBOOL EnumParser::readEnumsFromFile(QString fileName)
{
    enums.clear();
    file = new QFile(fileName);
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
                QMap<tUINT32,enumFromFile> tempEnums;
                //нашли первую группу
                while(!in.atEnd()){
                    line = in.readLine();
                    if(line.contains("}")){
                        break;
                    }

                    QString comment = "";
                    if(line.contains("=")){
                        subLine = line.mid(0,line.indexOf("="));
                        QRegExp rx("\\=[ \t]+\\d");
                        rx.indexIn(line);
                        QStringList list = rx.capturedTexts();
                        QString number = list.at(0);
                        counter = number.remove("=").trimmed().toInt();
                        subLine = subLine.trimmed();

                        if(line.contains("//")){
                            comment = line.mid(line.lastIndexOf("//"),line.size()-line.lastIndexOf("//"));
                        }

                        tempEnums.insert(counter,{subLine,comment});
                        counter++;

                    } else{
                        line = line.replace(",","");

                        if(line.contains("//")){
                            comment = line.mid(line.lastIndexOf("//"),line.size()-line.lastIndexOf("//"));
                            line = line.mid(0,line.indexOf("//"));
                        }
                        line = line.trimmed();
                        tempEnums.insert(counter,{line,comment});
                        counter++;
                    }
                }
                enums.append({enumName,tempEnums});
            }
        }
    } else{
        return false;
    }
    file->close();
    return true;
}

