#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QString>
#include <QTextBlock>
#include <QTextEdit>
#include <QTextDocument>
#include <QMessageBox>
#include <QVector>
#include <iostream>
//#include <Python.h>
#include "filterdata.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("人体下肢行为识别系统");
    this->threadFilter = new MyThreadFilter(this);
    this->threadLabel = new MyThreadLabel(this);
    this->threadDownstairs = new MyThreadLabelDownstairs(this);
    this->threadUpstairs = new MyThreadLabelUpstairs(this);
    this->threadAll = new MyThreadALL(this);
    this->threadjump = new MyThreadJump(this);
    this->threadUpHill = new MyThreadLabelUpHill(this);


    //==============平地行走 上下楼梯===========================
    connect(ui->LabelButton,&QPushButton::clicked,this,&MainWindow::ll);
    connect(threadAll,&MyThreadALL::isDone,this,&MainWindow::deal);
    //=============跳跃=============================
    connect(this,&MainWindow::walkupdownOver,this,&MainWindow::labelJump);
    connect(threadjump,&MyThreadJump::isDone,this,&MainWindow::showJump);
    connect(threadjump,&MyThreadJump::fileERROR,this,&MainWindow::FileError);
    //===================上坡==============================
    connect(this,&MainWindow::jumpOver,this,&MainWindow::labelUphill);
    connect(threadUpHill,&MyThreadLabelUpHill::isDone,this,&MainWindow::showhill);
    connect(threadUpHill,&MyThreadLabelUpHill::fileERROR,this,&MainWindow::FileError);

    connect(ui->model,&QPushButton::clicked,this,&MainWindow::callPython);

    ansfile = new QFile;
    ansfile->setFileName("./ansfile.csv");
    ansfile->open(QIODevice::WriteOnly);
    filein.setDevice(ansfile);
    //==================TCP连接========================================
    this->client_socket = new TcpClient;
    connect(ui->ConnectButton,&QPushButton::clicked,this,&MainWindow::TcpConnect);
    connect(ui->DisconnectButton,&QPushButton::clicked,this,&MainWindow::TcpDisconnect);
    connect(client_socket,&TcpClient::jump_answer,this,&MainWindow::online);
    connect(client_socket,&TcpClient::server_dis,this,&MainWindow::TcpServerDisconnect);
    connect(client_socket,&TcpClient::disconnected,this,&MainWindow::TcpServerDisconnect);
    ui->TimelineEdit->setReadOnly(true);
    ui->TimelineEdit->clear();
    ui->TimelineEdit->insert(QString("  %1 ms").arg(QString::number(43.1)));
    ui->TimelineEdit->setText(QString("%1 ms").arg(40.135));
    ui->TCPlineEdit->setText("成功连接到下位机系统");

    // walk ups dos uph dhi jum
    walkbuttons[0] = ui->pushButton_2;
    walkbuttons[1] = ui->pushButton_3;
    walkbuttons[2] = ui->pushButton_4;
    walkbuttons[3] = ui->pushButton_5;
    walkbuttons[4] = ui->pushButton_6;


    upstairsbuttons[0] = ui->pushButton_7;
    upstairsbuttons[1] = ui->pushButton_8;
    upstairsbuttons[2] = ui->pushButton_9;
    upstairsbuttons[3] = ui->pushButton_10;
    upstairsbuttons[4] = ui->pushButton_11;

    downstairsbuttons[0] = ui->pushButton_12;
    downstairsbuttons[1] = ui->pushButton_13;
    downstairsbuttons[2] = ui->pushButton_14;
    downstairsbuttons[3] = ui->pushButton_15;
    downstairsbuttons[4] = ui->pushButton_16;

    uphillbuttons[0] = ui->pushButton_17;
    uphillbuttons[1] = ui->pushButton_18;
    uphillbuttons[2] = ui->pushButton_19;
    uphillbuttons[3] = ui->pushButton_20;
    uphillbuttons[4] = ui->pushButton_21;

    downhillbuttons[0] = ui->pushButton_22;
    downhillbuttons[1] = ui->pushButton_23;
    downhillbuttons[2] = ui->pushButton_24;
    downhillbuttons[3] = ui->pushButton_25;
    downhillbuttons[4] = ui->pushButton_26;

    jumpbuttons[0] = ui->pushButton_27;
    jumpbuttons[1] = ui->pushButton_28;
    jumpbuttons[2] = ui->pushButton_29;
    jumpbuttons[3] = ui->pushButton_30;
    jumpbuttons[4] = ui->pushButton_31;


     ui->tabWidget->setCurrentIndex(0);

     buttons[0] = uphillbuttons;
     buttons[1] = downhillbuttons;
     buttons[2] = upstairsbuttons;
     buttons[3] = downstairsbuttons;
     buttons[4] = walkbuttons;
     buttons[5] = jumpbuttons;
     for(int i=0;i<6;++i){
         for(int j=0;j<5;++j){
             buttons[i][j]->set_normal();
         }
     }

      walkbuttons[1]->set_action();
}

MainWindow::~MainWindow()
{
    ansfile->close();
    delete ui;
}

void MainWindow::deal()
{

    qDebug()<<u8"===行走上下楼梯结束========";
    emit this->walkupdownOver();
}
void MainWindow::FileError(QString err)
{
    QMessageBox messageBox(QMessageBox::NoIcon,
                           "警告!", "离线识别完成！",
                           QMessageBox::Yes, NULL);
    messageBox.setText(err);
    messageBox.exec();
}
void MainWindow::labelWalk(){
    this->threadLabel->start();
}
void MainWindow::ll(){
    this->threadAll->start();
}
void MainWindow::labelDownstairs(){
    this->threadDownstairs->start();
}
void MainWindow::labelUpstairs(){
    this->threadUpstairs->start();
}
void MainWindow::labelUphill(){
    this->threadUpHill->start();
}


void MainWindow::labelJump(){
    threadjump->start();
}
void MainWindow::showJump(){
        qDebug()<<u8"==========跳跃完成结束============";
        emit this->jumpOver();
}
void MainWindow::showhill(){
    QMessageBox messageBox(QMessageBox::NoIcon,
                              "提示!", "全部完成！",
                              QMessageBox::Yes, NULL);
    messageBox.exec();
}

QString MainWindow::ReadQssFile(const QString &filePath)
{
    QString strStyleSheet = "";
    QFile file(filePath);
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        strStyleSheet = QLatin1String(file.readAll());
    }
    return  strStyleSheet;
}


//==========================TCP==================================
void MainWindow::TcpConnect()
{
    QString ip = ui->IPlineEdit->text();
    if(ip.split('.').size() != 4)
    {
        QMessageBox messageBox(QMessageBox::NoIcon,
                               "警告!", "请输入正确IP地址，如192.168.3.14",
                               QMessageBox::Yes, NULL);
        messageBox.exec();
        return;
    }
    qint16 port = ui->PortlineEdit->text().toInt();
    client_socket->connectToHost(QHostAddress(ip),port);
    if(client_socket->waitForConnected(1000))
    {
        ui->DisconnectButton->setEnabled(true);
        ui->ConnectButton->setEnabled(false);
        ui->TCPlineEdit->setText("成功连接到下位机系统");
    }
}
void MainWindow::TcpDisconnect()
{
    client_socket->disconnectFromHost();//主动断开（主动断开或则被动断开都会出发disconnected信号）
}

void MainWindow::TcpServerDisconnect()
{

    ui->DisconnectButton->setEnabled(false);
    ui->ConnectButton->setEnabled(true);
    ui->TCPlineEdit->setText("未连接到下位机系统");
    qDebug()<<("连接断开");
    ui->tabWidget->setCurrentIndex(0);
    for(int i=0;i<6;++i){
        for(int j=0;j<5;++j){
            buttons[i][j]->set_normal();
        }
    }

}
void MainWindow::online(int label1,int label2, float time)
{
   if(label1 == -1){
       ui->tabWidget->setCurrentIndex(0);
       for(int i=0;i<6;++i){
           for(int j=0;j<5;++j){
               buttons[i][j]->set_normal();
           }
       }
       ui->TimelineEdit->setText(QString("%1 ms").arg(0));
       return;
   }
   this->filein<<label1<<','<<label2<<','<<time<<Qt::endl;
   ui->TimelineEdit->setText(QString("%1 ms").arg(time));
   int tabindex = 0;
   switch(label1){
   case 0: {
       tabindex = 3;
       break;
   }
   case 1: {
       tabindex = 4;
       break;
   }
   case 2: {
       tabindex = 1;
       break;
   }
   case 3: {
       tabindex = 2;
       break;
   }
   case 4: {
       tabindex = 0;
       break;
   }
   case 5: {
       tabindex = 5;
       break;
   }
   default:break;
   }
   ui->tabWidget->setCurrentIndex(tabindex);
   int v=(label2+2)%5;
   if(label1 == 5){
       v = (label2+4)%5;
   }
   for(int i=0;i<5;++i){
       if(i == v){
           buttons[label1][i]->set_action();
       }else{
           buttons[label1][i]->set_normal();
       }
   }

}


void MainWindow::callPython(){

//    //指定好Python的库和解释器的位置
//    Py_SetPythonHome((const wchar_t *)(L"D:/anaconda/envs/tfgpu2_1"));
//    Py_Initialize();
//    if (!Py_IsInitialized()) {
//        printf("inititalize failed");
//        qDebug() << "inititalize failed";
//    }
//    else {
//        qDebug() << "inititalize success";
//    }
//    PyRun_SimpleString("import sys");
//    PyRun_SimpleString("sys.path.append('./')");
//    PyObject *pModule = PyImport_ImportModule("demo");
//    if(pModule)
//    {
//        PyObject *pLoadFunc2 = PyObject_GetAttrString(pModule,"TrainAndConvert");
//        PyObject_CallObject(pLoadFunc2,NULL);
//    }
//    else
//    {
//         qDebug() << "load module failed";
//    }
//    Py_Finalize();//释放python解释器的资源
}
