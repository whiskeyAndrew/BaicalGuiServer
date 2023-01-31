#include "enumparser.h"

//Парсер енамов
//Берет список енамов из текстовика (енамы должны писаться "как в плюсах"), пишет их в контейнер и передает в traceWindowSettings

//------------АЙДИШНИКИ ЕНАМОВ ГЕНЕРИРУЮТСЯ В КОНСТРУКТОРЕ------------//

//один из тех случаев, где написано очень много нечитабельного кода из-за многих решений, принятых в начале написания проекта


EnumParser::EnumParser()
{
    QMap<tUINT32,enumFromFile> nullEnum;
    //филлер
    nullEnum.insert(0,{"NULL","NULL"});

    //0 ID - енам не нужен
    enums.append({"0",nullEnum});

    //Уникальные ЕНАМЫ, нужны для уникальной обработки аргументов
    //Здесь добавляется уникальный енам и автоматически присваивается ему айдишник, диапазон 1-100
    //Если нужен свой обработчик аргументов, то просто нужно добавить его сюда, указав первым элементов название

    //Для реализации алгоритма обработки смотри traceWindow.cpp -> getGuiRow, там здоровенный 9-тиэтажный комментарий
    //Вторую часть енама обязательно делать nullEnum
    enums.append({"1234567 -> 1 234 567",nullEnum}); //id 1
    enums.append({"321234567890->321.234567890",nullEnum}); //id 2

    while(enums.size()<101){
        enums.append({"0",nullEnum});
    }
}

tBOOL EnumParser::readEnumsFromFile(QString fileName)
{
    for(int i =enums.size();i>100;i--){
        enums.removeAt(i);
    }

    file = new QFile(fileName);

    //Резервируем 100 айдишников для модулей, которые будут вписываться вручную


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
                enums.insert(100+enums.size(),{enumName,tempEnums});
            }
        }
    } else{
        return false;
    }
    file->close();
    return true;
}

