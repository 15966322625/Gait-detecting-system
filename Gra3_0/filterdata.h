#ifndef FILTERDATA_H
#define FILTERDATA_H

/***************************************************************************************
                               数据滤波
***************************************************************************************/
#include <QFile>
#include <QString>
class FilterData
{
public:
    FilterData(QString EMGpath, QString IMUpath,QString FOOTpath);
    ~FilterData();
    int FilterEMGData();//分别实现对三种数据的滤波处理
    int FilterIMUData();
    int FilterFOOTData(int);
private:
    QFile* EMG;
    QFile* IMU;
    QFile* FOOT;
    QString EMGpath;
    QString IMUpath;
    QString FOOTpath;
};
#endif // FILTERDATA_H


