#ifndef IMUPITCHDETECT_H
#define IMUPITCHDETECT_H

/***************************************************************************************
                              寻找IMU峰值
***************************************************************************************/

#include <QString>
#include <QFile>
#include "Configure.h"

struct pitch_node
{
    int point; //记录4个点中第4个点的序号
    float value;//记录4个点的平均值
};
struct record_pitch_max
{
    int flag; //目前状态 是寻找逐渐变大的点 还是逐渐变小的点
    float pitch_record;
    int point;
    pitch_node pitch[WIDTH];
};
class IMUPitchDetect
{
private:
    int begin;//活动段开始点
    int end;	//活动段结束点
    QFile *IMU_file;
    pitch_node pitch_mean[WIDTH]; //记录pitch变化窗口 用来更新和判断pitch数据进行判断峰值点
    record_pitch_max record;
public:
    IMUPitchDetect(const QString&,int begin,int end);
    ~IMUPitchDetect();
public:
    void set_begin_end(int,int);
    bool peak_judge(pitch_node*);//计算峰值点
    bool valley_judge(pitch_node*);//计算谷底值点
    void read_imu_data();//总函数  读文件 求均值 填入pitch_mean 判断 刷新pitch
    void baselineIMU();
    void pitch_mean_appen(int,float);
    void move_window();//更新窗，前一半等于上一个的后一半
    int peak[10]; //记录变化峰值点
    int peak_count; //记录活动段中有几个峰值点
    int valley[10]; //记录变化谷底
    int valley_count;	//记录活动段中有几个谷底点
};

#endif // IMUPITCHDETECT_H
