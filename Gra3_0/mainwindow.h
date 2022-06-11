#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QString>
#include <vector>
#include <QFile>
#include "MyThread.h"
#include "tcpclient.h"
#include "mypushbutton.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void deal();
    void FileError(QString);
    void ll();
    void labelWalk();
    void labelUpstairs();
    void labelDownstairs();
    void labelUphill();
    void ALL();
    void labelJump();
    void showJump();
    void showhill();
    void callPython();
    QString ReadQssFile(const QString& filePath);
public:
    MyThreadFilter* threadFilter;
    MyThreadLabel* threadLabel;
    MyThreadLabelUpstairs* threadUpstairs;
    MyThreadLabelDownstairs* threadDownstairs;
    MyThreadALL* threadAll;
    MyThreadJump* threadjump;
    MyThreadLabelUpHill* threadUpHill;

private:
    Ui::MainWindow *ui;

private:
    //====tcp=========
    TcpClient *client_socket;
    QFile* ansfile;
    QTextStream filein;
    void TcpConnect();
    void TcpDisconnect();
    void online(int u, int v, float time);
    void TcpServerDisconnect();

    MyPushbutton* jumpbuttons[5];
    MyPushbutton* walkbuttons[5];
    MyPushbutton* uphillbuttons[5];
    MyPushbutton* downhillbuttons[5];
    MyPushbutton* upstairsbuttons[5];
    MyPushbutton* downstairsbuttons[5];

    MyPushbutton** buttons[6];
signals:
    void walkupdownOver();
    void jumpOver();
    void updownhillOver();
};
#endif // MAINWINDOW_H
