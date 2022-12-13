#include "includes/launcher.h"
#include "mainwindow.h"
#include <QApplication>
#include "includes/confighandler.h"
/*Туду:
-  (вроде сделано) допилить чтение аргументов, есть проблема что если мы передаем .f то оно неправильно пишет стринг, надо придумать какую-то побайтовую запись в идеале
-  переписать обработку входящих клиентов на адекватную реализацию вместо тестовой
на данный момент клиент в ГУИ - это (зеленый круг или красный крест)+sin_addr:sin_port, по этому паттерну вся программа читает соединения, мне не очень
нравится эта реализация, надо будет переписать
- При первом подключении нужно запилить отправку приветственного пакета, если мы впервые видим клиента
Это помогает избежатьь вариантов, Когда отключается байкал и коннект сразу переходит на этот сервак
-  допилить везде деструкторы при закрытии окна, чтобы мы при закрытии виджета удаляли весь ненужный мусор из памяти
-  проверка CRC у каждого пришедшего пакета, к тому же надо разобраться че делать если црц неправильно считается
-  динамическое построение ответа клиенту, сейчас некоторые отправляемые в ответ переменные статические
-  придумать как красиво передавать текст в гую чтобы он всегда мог помещаться на экране
*/

int main(int argc, char* argv[])
{
    qRegisterMetaType<sockaddr_in>("sockaddr_in");
    qRegisterMetaType<tUINT32>("tUINT32");
    qRegisterMetaType<TraceToGUI>("TraceToGUI");
    qRegisterMetaType<std::queue<TraceToGUI>>("std::queue<TraceToGUI>");
    qRegisterMetaType<UniqueTraceData>("UniqueTraceData");
    qRegisterMetaType<PacketHandler*>("PacketHandler*");
    QApplication a(argc, argv);

    MainWindow mainWindow;
    Launcher* launcher = new Launcher(&mainWindow);
    mainWindow.launcher = launcher;

    launcher->start();
    mainWindow.show();

    a.exec();

    //Все окна закрыты, завершаем все потоки
    a.closeAllWindows();
    launcher->requestInterruption();
    launcher->wait();

    std::cout<<"------"<<"Main thread closing"<<"------"<<std::endl;
}

