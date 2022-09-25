#include "includes/launcher.h"
#include "mainwindow.h"
#include <QApplication>

/*Туду:
-  (вроде сделано) допилить чтение аргументов, есть проблема что если мы передаем .f то оно неправильно пишет стринг, надо придумать какую-то побайтовую запись в идеале
-  переписать обработку входящих клиентов на адекватную реализацию вместо тестовой
-  допилить везде деструкторы при закрытии окна, чтобы мы при закрытии виджета удаляли весь ненужный мусор из памяти
-  проверка CRC у каждого пришедшего пакета, к тому же надо разобраться че делать если црц неправильно считается
-  динамическое построение ответа клиенту, сейчас некоторые отправляемые в ответ переменные статические
-  придумать как красиво передавать текст в гую чтобы он всегда мог помещаться на экране
*/

int main(int argc, char *argv[])
{
    qRegisterMetaType<sockaddr_in>("sockaddr_in");
    qRegisterMetaType<tUINT32>("tUINT32");
    qRegisterMetaType<TraceToGUI>("TraceToGUI");

    QApplication a(argc, argv);

    MainWindow mainWindow;
    Launcher launcher;
    mainWindow.launcher = &launcher;
    launcher.SetMainWindow(&mainWindow);
    launcher.start();

    mainWindow.show();

    return a.exec();
}

