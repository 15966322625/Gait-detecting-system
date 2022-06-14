#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;//主界面
    QString buttonstyle=w.ReadQssFile(":/Qss/Qss/My_Qss.qss");//图片位置
    a.setStyleSheet(buttonstyle);
    w.show();
    return a.exec();
}












