#ifndef ACTIVESECTION_H
#define ACTIVESECTION_H
/***************************************************************************************
    检测活动段
    ActiveSectionDetection检测疑似跳跃活动段的起始位置
    AllActiveSectionDetection检测所有的熵大于阈值的活动段
***************************************************************************************/

#include <QFile>
#include <QTextStream>
#include <QString>
#include "Configure.h"
#include "MyDataStruct.h"

class ActiveSection
{
public:
    ActiveSection(QString);
    ~ActiveSection();
    void clear();
    void clear_slpw();
    void MK_slpw();  //更新构建滑动窗
    void imu_MK_slpw();  //更新iimu构建滑动窗
    float SampEn_Br(float*, int, float, int);//样本熵公式
    void SampEnCal(); // 计算样本熵
    void imu_SampEnCal();// 计算imu样本熵
    void open_file_samp();
    void open_file_all_samp();
    void open_file_imu_samp();
    void ADC_append(float*);   //DATA追加数据
    void IMU_append(float[8][12]);//IMU追加数据
    bool data_full();               //判断数据包是否满了
    bool check_action(); //检测活动段跳跃起点
    bool check_action_2();//检测活动段起点
    bool check_action_IMU();
    bool check_still(); //检测活动段跳跃终点
    bool check_still_2();//检测活动段终点
    bool check_still_IMU();
    int check(); //检测活动段中跳跃的点，返回活动段 开始/结束 节点
    int check_2(); //检测活动段
    void imu_check();//imu
    bool status;
    bool ACT_STATUS;
    float ACC_HIGH;//新增部分
    float ACC_LOW;//新增部分
private:
    ACT_DATA* DATA; //数据包40个adc数据，4个imu数据
    float SLPW_buf[SLPW_LEN];//熵的滑动窗
    float SampEn_num;          //熵
    float imu_SLPW_buf[6]; //熵的滑动窗
    float imu_SampEn_num;      //熵
    bool flag;                //状态 true：活动段中 false：不在活动段
    bool jump_flag;            //记录跳跃 true检测到跳跃起点
    int begin;              //记录活动段 开始/结束 节点
    int end;
    int ACT_BEGIN;
    int ACT_END;
    QFile file_samp;
    QTextStream out;
    QString sampEN_path;
    QFile file_samp_2;
    QTextStream out_2;
    QString sampEN_path_2;
    //imu
    QFile imu_file_samp;
    QTextStream imu_out;
    QString imu_sampEN_path;
};
int ActiveSectionDetection(ACT_point&,QString,QString,QString,int,float,float);
int AllActiveSectionDetection(ACT_point&,QString,QString,QString,float,float);
#endif // ACTIVESECTION_H
