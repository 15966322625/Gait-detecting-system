#ifndef DATASET_H
#define DATASET_H

/***************************************************
    通过得出的6个节点计算出5种步态数据库
****************************************************/
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <random>
#include <iterator>
#include "MyDataStruct.h"
class Dataset
{
public:
    Dataset();
    Dataset(QString adc_path, QString imu_path,QString EMGfeaturepath,QString IMUfeaturepath,QString actionLabelpath,QString gaitpath);
    ~Dataset();
    int file_skip(int,int);	//跳转到动作数据开始行
    void init_file();				//重置文件到开头位置
    void clear_ACTD();
    void write_feature(int label,int action);
    void move_forward();			//以步长移动窗
    int datasetcal(std::vector<std::vector<int>>& tag,int action); //制作数据集
    int datasetUpDownHillcal(std::vector<std::vector<int>>& tag,int action); //制作上下坡的数据集，由于上下坡EMG数据和IMU数据有点便宜，所以需要特殊处理
public:
    ACT_DATA* ACTD;

    QFile* ADC;
    QTextStream ADC_in;
    QFile* IMU;
    QTextStream IMU_in;
    QFile* EMGfeature;
    QTextStream EMGfeature_out;
    QFile* IMUfeature;
    QTextStream IMUfeature_out;
    QFile* gait;
    QTextStream gait_out;
    QFile* action;
    QTextStream action_out;
    QString EMGfeapath;
    QString IMUfeapath;
    QString gaitfeapath;
    QFile EMGfea[5];
    QFile IMUfea[5];
    QFile gaitfea[5];

    QTextStream EMGout[5];
    QTextStream IMUout[5];
    QTextStream gaitout[5];

    QFile EMGfeagaussian[5];
    QFile IMUfeagaussian[5];
    QTextStream EMGoutgaussian[5];
    QTextStream IMUoutgaussian[5];

    QFile EMGfea09[5];
    QFile IMUfea09[5];
    QTextStream EMGouttimescal0_9[5];
    QTextStream IMUouttimescal0_9[5];

    QFile EMGfea11[5];
    QFile IMUfea11[5];
    QTextStream EMGouttimescal1_1[5];
    QTextStream IMUouttimescal1_1[5];


};

#endif // DATASET_H
