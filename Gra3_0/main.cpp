#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QString buttonstyle=w.ReadQssFile(":/Qss/Qss/My_Qss.qss");
    a.setStyleSheet(buttonstyle);
    w.show();
    return a.exec();
}












