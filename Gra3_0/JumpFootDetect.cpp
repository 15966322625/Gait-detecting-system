#include "JumpFootDetect.h"
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
JumpFootDetect::JumpFootDetect(const QString& path,int ch,int peak_1,int peak_2)
{
    this->foot_path = path;
    this->Foot = new QFile;
    this->rise_count = 0;
    memset(rise_begin, 0, sizeof(rise_begin));
    memset(rise_end, 0, sizeof(rise_end));
    this->CH = ch;
    flag = false;
    this->peak_1 = peak_1;
    this->peak_2 = peak_2;
}
JumpFootDetect::~JumpFootDetect(){
    delete Foot;
    Foot = nullptr;
}
void JumpFootDetect::check()
{
    int record_value = 0;
    int record_point = 0;
    int count = peak_1;
    Foot->setFileName(this->foot_path);
    if (!Foot->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, u8"文件出错", u8"脚底压力文件打开出错");
        exit(-1);
    }
    QTextStream foot_in(Foot);
    for (int i = 0; i < this->peak_1-1; ++i)
    {
        if (foot_in.atEnd())
        {
            //QMessageBox::information(NULL, u8"文件", u8"脚底压力over");
            Foot->close();
            qDebug()<<"peak_1:" <<peak_1<<"通道："<<this->CH<<"脚底压力over JumpFootDetect::check()";
            return;
        }
        foot_in.readLine();
    }
    while (count <= peak_2 && (!foot_in.atEnd()))//在两次下蹲之间进行判断
    {
        QString foot_str = foot_in.readLine();
        count++;
        auto foot_list = foot_str.split(',');
        int point = foot_list.at(0).toInt();
        int value = foot_list.at(this->CH).toInt();
        if (!this->flag)
        {
            if (value != record_value && value == 1)
            {
                this->rise_begin[rise_count] = point;
                this->flag = true;
                record_value = value;
                record_point = point;
            }
            else
            {
                record_value = value;
                record_point = point;
            }
        }
        else
        {
            if (value != record_value && value == 0)
            {
                this->rise_end[rise_count] = record_point;
                this->flag = false;
                rise_count++;
                record_value = value;
                record_point = point;
            }
            else
            {
                record_value = value;
                record_point = point;
            }
        }
    }
    foot_in.flush();
    Foot->close();
}
