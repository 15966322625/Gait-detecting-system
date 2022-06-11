#include "tcpclient.h"
TcpClient::TcpClient(QObject *parent)
{
    connect(this,&QTcpSocket::readyRead,this,&TcpClient::datareceive);
    connect(this,&QTcpSocket::disconnected,this,&TcpClient::server_disconnect);
}
void TcpClient::datareceive()
{
    QByteArray buff = this->readAll();
    ANS *buf = (ANS*)(buff.data());
    int label1 = buf->U;
    int label2 = buf->V;
    float time = buf->time_process;
    long int tsec = buf->sec;
    long int tusec = buf->usec;
    emit jump_answer(label1,label2,time);
}
void TcpClient::server_disconnect()
{
    emit server_dis(this->socketDescriptor());
}
