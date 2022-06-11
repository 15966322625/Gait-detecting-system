#ifndef JUMPFOOTDETECT_H
#define JUMPFOOTDETECT_H

/***************************************************************************************
    利用足底压力的4个传感器 判断一次跳跃空中的端点
    使用第四个通道的压力效果最好（怀疑是新的鞋套的前脚掌）
***************************************************************************************/

#include <QString>
#include <QFile>

class JumpFootDetect
{
public:
    JumpFootDetect(const QString&,int,int,int);
    ~JumpFootDetect();
public:
    void check();	//检测总函数
public:
    int rise_begin[10];
    int rise_end[10];
    int rise_count; // 记录几次腾空
private:
    bool flag;		//false 代表现在没在空中，寻找腾空起点 找到后变为true，寻找腾空结束点
    int peak_1;
    int peak_2;		//一次完整跳跃中的两次下蹲最低点
    QFile* Foot;
    QString foot_path;
    int CH;		//第几个通道的足底压力
};

#endif // JUMPFOOTDETECT_H
