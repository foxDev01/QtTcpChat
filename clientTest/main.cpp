#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Клиент");
    w.setGeometry(100,100,500,300);
    w.show();
    return a.exec();
}
