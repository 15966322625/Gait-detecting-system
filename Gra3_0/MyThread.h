#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QMainWindow>
#include <QThread>
#include <QString>
#include <QMutex>
#include <vector>
#include "FootDetect.h"
#include "MyDataStruct.h"
//============数据滤波===================
class MyThreadFilter : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadFilter(QObject* parent = nullptr);
    ~MyThreadFilter();
    int filenum;
    void setfile(int n);
protected:
    void run();//重写线程函数
signals:
    void isDone();
    void fileERROR(QString err);
};

//============自动打标签 walk=====================
class MyThreadLabel : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadLabel(QObject* parent = nullptr);
    ~MyThreadLabel();
    FootDetect foot;
    std::vector<int> action;
    void SetAction(int);
protected:
    void run();//重写线程函数
signals:
    void isDone();
    void fileERROR(QString err);
};

//============自动打标签 上楼梯=====================
class MyThreadLabelUpstairs : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadLabelUpstairs(QObject* parent = nullptr);
    ~MyThreadLabelUpstairs();
    FootDetect foot;
    std::vector<int> action;
    void SetAction(int);
protected:
    void run();//重写线程函数
signals:
    void isDone();
    void fileERROR(QString err);
};

//============自动打标签 下楼梯=====================
class MyThreadLabelDownstairs : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadLabelDownstairs(QObject* parent = nullptr);
    ~MyThreadLabelDownstairs();
    FootDetect foot;
    std::vector<int> action;
    void SetAction(int);
protected:
    void run();//重写线程函数
signals:
    void isDone();
    void fileERROR(QString err);
};

//============批量行走、上楼梯、下楼梯====================
class MyThreadALL : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadALL(QObject* parent = nullptr);
    ~MyThreadALL();
    FootDetect foot;
    std::vector<int> action;
    void SetAction(int);
    void recordfile(QString path, std::vector<std::vector<int>> &gait,int);
protected:
    void run();//重写线程函数
signals:
    void isDone();
    void fileERROR(QString err);
};

//==================检测跳跃==============
class MyThreadJump : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadJump(QObject* parent = nullptr);
    ~MyThreadJump();
protected:
    void run();//重写线程函数
    void findband();
signals:
    void isDone();
    void fileERROR(QString err);


public:
    QString EMG_original_path;
    QString imu_original_path;
    QString foot_original_path;
    QString EMG_filtered_path;
    QString imu_filtered_path;
    QString foot_filtered_path;
    QString samp_path;
    QString dataset_path;
    jump* MyJump;
    ACT_point point;
    ACT_point point_2;
    ALL_point *action;
    ALL_point *actionpre; //处理后的点，进一步检测action中的点是否有0点
    std::vector<std::vector<int>> gaitJump;
    float HIGH;
    float LOW;
public:
    int detect_ACT();
    void detect_valley();
    void detect_rise();
    void integrate_action();//检测一整个活动段

};

//============自动打标签 上坡=====================
class MyThreadLabelUpHill : public QThread
{
    Q_OBJECT
public:
    explicit MyThreadLabelUpHill(QObject* parent = nullptr);
    ~MyThreadLabelUpHill();
    FootDetect foot;
    std::vector<int> action;
    void SetAction(int);
protected:
    void run();//重写线程函数
    void recordfile(QString path, std::vector<std::vector<int>> &gait,int);
signals:
    void isDone();
    void fileERROR(QString err);
};

#endif // MYTHREAD_H
