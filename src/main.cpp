#include "headers/mainwindow.h"
#include "headers/systemcall.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    SystemCall w;
    w.resize(700, 500);
    w.show();

    return a.exec();
}
