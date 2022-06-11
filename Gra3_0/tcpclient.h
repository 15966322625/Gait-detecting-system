#ifndef TCPCLIENT_H
#define TCPCLIENT_H

/***************************************************************************************
                               网络类 接受下位机传输的信号
***************************************************************************************/

#include <QObject>
#include<QTcpSocket>
#include<QHostAddress>
#include<QMessageBox>

struct ANS
{
       int U;
       int V;
       float time_process;
       long int sec;
       long int usec;
};
class TcpClient:public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    void datareceive();
    void server_disconnect();
signals:
    void jump_answer(int label1,int label2, float time);
    void server_dis(int);
};

#endif // TCPCLIENT_H
