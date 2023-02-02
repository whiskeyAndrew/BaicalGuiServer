#include "includes/launcher.h"
#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
/*Туду:
-  (вроде сделано) допилить чтение аргументов, есть проблема что если мы передаем .f то оно неправильно пишет стринг, надо придумать какую-то побайтовую запись в идеале
-  проверка CRC у каждого пришедшего пакета, к тому же надо разобраться че делать если црц неправильно считается
-  динамическое построение ответа клиенту, сейчас некоторые отправляемые в ответ переменные статические
*/

//В мэине задаются метатипы для сигналов, старые версии QT не могут прокидывать сигналы если типы передаваемых данных не объявлены в метатайпе
//Запускается лаунчер и главное окно, лаунчер отвечает за получение пакетов по ЮДП и передаче их на обработку

//На будущее -> было бы суперудобно если бы был объект Client, который хранил бы в себе данные по каждому клиенту. Его отсутствие ставит мне миллионы палок в колеса
//И его внедрение надо было делать в самом начале создания проекта
//Опыт разработки - классная штука, особенно когда понимаешь, что некоторые вещи на старте могли бы тебе упростить разработку в миллионы раз

int main(int argc, char* argv[])
{
    qRegisterMetaType<sockaddr_in>("sockaddr_in");
    qRegisterMetaType<tUINT32>("tUINT32");
    qRegisterMetaType<GUIData>("GUIData");
    qRegisterMetaType<UniqueTraceData>("UniqueTraceData");
    qRegisterMetaType<PacketHandler*>("PacketHandler*");
    qRegisterMetaType<sP7Trace_Module>("sP7Trace_Module");
    QApplication a(argc, argv);

    //Использование русских символов
    QTextCodec * codec;
    codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    MainWindow mainWindow;
    Launcher* launcher = new Launcher(&mainWindow);

    mainWindow.setLauncher(launcher);

    launcher->start();
    mainWindow.show();

    a.exec();

    //Все окна закрыты, завершаем все потоки
    a.closeAllWindows();
    launcher->requestInterruption();
    launcher->wait();

    std::cout<<"------"<<"Main thread closing"<<"------"<<std::endl;
}

