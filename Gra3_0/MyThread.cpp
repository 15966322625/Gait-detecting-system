#include "MyThread.h"
#include "filterdata.h"
#include "FootDetect.h"
#include <vector>
#include <QDebug>
#include <QTextStream>
#include "Dataset.h"
#include "ActiveSection.h"
#include "JumpFootDetect.h"
#include "IMUPitchDetect.h"
MyThreadFilter::MyThreadFilter(QObject *parent): QThread(parent)
{
    filenum = 1;
}
MyThreadFilter::~MyThreadFilter(){}
void MyThreadFilter::run(){
    QString pathEMG = QString("C:/Users/15492/Desktop/walk/yuya/walk_up_down/%1/file_ADC.csv").arg(filenum);
    QString pathIMU = QString("C:/Users/15492/Desktop/walk/yuya/walk_up_down/%1/file_IMU.csv").arg(filenum);
    QString pathFOOT = QString("C:/Users/15492/Desktop/walk/yuya/walk_up_down/%1/file_FOOT.csv").arg(filenum);
    FilterData data(pathEMG,pathIMU,pathFOOT);
    int res1 = data.FilterEMGData();
    int res2 = data.FilterIMUData();
    int res3 = data.FilterFOOTData(50);
    if(res1<0){
        emit fileERROR("EMG filter error");
        return;
    }
    if(res2 < 0){
        emit fileERROR("IMU filter error");
        return;
    }
    if(res3 < 0){
        emit fileERROR("foot filter error");
        return;
    }
    emit isDone();
}
void MyThreadFilter::setfile(int n){
    filenum = n;
}

//=============================================================
MyThreadLabel::MyThreadLabel(QObject *parent): QThread(parent)
{
    action.push_back(0);
    action.push_back(3);
}
void MyThreadLabel::SetAction(int n){
    action.push_back(n);
}
MyThreadLabel::~MyThreadLabel(){}
void MyThreadLabel::run(){
    foot.FindAirFootPoint(1,foot.ch1);
    foot.FindAirFootPoint(2,foot.ch2);
    foot.FindHeelToe();
    std::vector<int> nums={34};
    foot.FindFiveGaitphasePoint(action,nums);
    emit isDone();
}

//=============================================================
MyThreadLabelUpstairs::MyThreadLabelUpstairs(QObject *parent): QThread(parent)
{
    action.push_back(2);
}
void MyThreadLabelUpstairs::SetAction(int n){
    action.push_back(n);
}
MyThreadLabelUpstairs::~MyThreadLabelUpstairs(){}
void MyThreadLabelUpstairs::run(){
    foot.FindAirFootPoint(1,foot.ch1);
    foot.FindAirFootPoint(2,foot.ch2);
    foot.FindHeelToe();
    std::vector<int> nums={22};
    foot.FindUpStairsGaitphasePoint(action,nums);
    emit isDone();
}

//=============================================================
MyThreadLabelDownstairs::MyThreadLabelDownstairs(QObject *parent): QThread(parent)
{
    action.push_back(1);
}
void MyThreadLabelDownstairs::SetAction(int n){
    action.push_back(n);
}
MyThreadLabelDownstairs::~MyThreadLabelDownstairs(){}
void MyThreadLabelDownstairs::run(){
    foot.FindAirFootPoint(1,foot.ch1);
    foot.FindAirFootPoint(2,foot.ch2);
    foot.FindHeelToe();
    std::vector<int> nums={58};
    foot.FindDownStairsGaitphasePoint(action,nums);
    emit isDone();
}

//=============================================================
MyThreadALL::MyThreadALL(QObject *parent): QThread(parent)
{
    action.push_back(1);
}
void MyThreadALL::SetAction(int n){
    action.push_back(n);
}
MyThreadALL::~MyThreadALL(){}
void MyThreadALL::run(){
    //取第15组实验作为在线数据
    std::vector<int> lanqingfilenums = {1,2,4,5,6,8,9,10,11,12,13,14,16,17,18,19,20,21,23};
    std::vector<int> guoxiangfilenums = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,17,18,19,20,21};
    std::vector<int> yuyafilenums = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,17,18,19};
    std::vector<int> zhangliangfilenums = {2,3,4,5,6,7,8,9,11,13,16,19,21,22};
    std::vector<int> bofilenums = {1,2,3,4,6,7,8,10,11,13,14,15,16};
    //std::vector<int> bofilenums = {17,18};
    std::vector<int> filenums = {7};
    QString allpathemg[5] = {"yuya","lanqing","guoxiang","zhangliang","bo"};
    std::vector<std::vector<int>> allfilenums;
    allfilenums.push_back(yuyafilenums);
    allfilenums.push_back(lanqingfilenums);
    allfilenums.push_back(guoxiangfilenums);
    allfilenums.push_back(zhangliangfilenums);
    allfilenums.push_back(bofilenums);
    QString peoples[5] = {"yu","lan","guo","zhang","bo"};
    //只更改保存的
    for(int peo=4;peo<5;++peo){
        for(int i:allfilenums[peo]){
            //数据滤波
            QString pathEMG = QString("C:/Users/15492/Desktop/data/walk/%1/walk_up_down/%2/file_ADC.csv").arg(allpathemg[peo]).arg(i);
            QString pathIMU = QString("C:/Users/15492/Desktop/data/walk/%1/walk_up_down/%2/file_IMU.csv").arg(allpathemg[peo]).arg(i);
            QString pathFOOT = QString("C:/Users/15492/Desktop/data/walk/%1/walk_up_down/%2/file_FOOT.csv").arg(allpathemg[peo]).arg(i);
            FilterData data(pathEMG,pathIMU,pathFOOT);
            qDebug()<<pathEMG<<Qt::endl;
            int res1 = data.FilterEMGData();
            int res2 = data.FilterIMUData();
            int res3 = data.FilterFOOTData(50);
            if(res1<0){
                emit fileERROR("EMG filter error");
                return;
            }
            if(res2 < 0){
                emit fileERROR("IMU filter error");
                return;
            }
            if(res3 < 0){
                emit fileERROR("foot filter error");
                return;
            }

            //
            foot.FindAirFootPoint(1,foot.ch1);
            foot.FindAirFootPoint(2,foot.ch2);
            foot.FindHeelToe();
            //行走不变，{34}改为leftshank
            std::vector<int> numswalk={70};
            std::vector<int> actionwalk = {0,3};
            foot.FindFiveGaitphasePoint(actionwalk,numswalk);

            QString targetlabelwalkpath = QString("./FilterData/%1/walklabel.csv").arg(peoples[peo]);
            QString targetEMGwalkpath = QString("./Dataset/%1/walkEMGfeature.csv").arg(peoples[peo]);
            QString targetIMUwalkpath = QString("./Dataset/%1/walkIMUfeature.csv").arg(peoples[peo]);
            QString targetactionwalkpath = QString("./Dataset/%1/walkaction.csv").arg(peoples[peo]);
            QString targetgaitwalkpath = QString("./Dataset/%1/walkgaitphase.csv").arg(peoples[peo]);

            recordfile(targetlabelwalkpath,foot.GaitPhase,i);
            Dataset walkdataset("./FilterData/filter_ADC.csv","./FilterData/filterori_IMU.csv",targetEMGwalkpath,targetIMUwalkpath,targetactionwalkpath,targetgaitwalkpath);
            walkdataset.datasetcal(foot.GaitPhase,4);
            //下楼梯 从numsdown={58} 改为numsdown={46,58} 算leftknee最小值
            std::vector<int> numsdown={58, 70};
            std::vector<int> actiondown = {1};
            foot.FindDownStairsGaitphasePoint(actiondown,numsdown);

            QString targetlabelDownStairspath = QString("./FilterData/%1/Downstairslabel.csv").arg(peoples[peo]);
            QString targetEMGDownStairspath = QString("./Dataset/%1/DownStairsEMGfeature.csv").arg(peoples[peo]);
            QString targetIMUDownStairspath = QString("./Dataset/%1/DownStairsIMUfeature.csv").arg(peoples[peo]);
            QString targetactionDownStairspath = QString("./Dataset/%1/DownStairsaction.csv").arg(peoples[peo]);
            QString targetgaitDownStairspath = QString("./Dataset/%1/DownStairsgaitphase.csv").arg(peoples[peo]);

            recordfile(targetlabelDownStairspath,foot.GaitPhaseDownStairs,i);
            Dataset DownStairsdataset("./FilterData/filter_ADC.csv","./FilterData/filterori_IMU.csv",targetEMGDownStairspath,targetIMUDownStairspath,targetactionDownStairspath,targetgaitDownStairspath);
            DownStairsdataset.datasetcal(foot.GaitPhaseDownStairs,3);
            //上楼梯改为numsdown={46,58} 算leftknee 最小值
            std::vector<int> numsup={58, 70};
            std::vector<int> actionup = {2};
            foot.FindUpStairsGaitphasePoint(actionup,numsup);

            QString targetlabelUpStairspath = QString("./FilterData/%1/Upstairslabel.csv").arg(peoples[peo]);
            QString targetEMGUpStairspath = QString("./Dataset/%1/UpStairsEMGfeature.csv").arg(peoples[peo]);
            QString targetIMUUpStairspath = QString("./Dataset/%1/UpStairsIMUfeature.csv").arg(peoples[peo]);
            QString targetactionUpStairspath = QString("./Dataset/%1/UpStairsaction.csv").arg(peoples[peo]);
            QString targetgaitUpStairspath = QString("./Dataset/%1/UpStairsgaitphase.csv").arg(peoples[peo]);

            recordfile(targetlabelUpStairspath,foot.GaitPhaseUpStairs,i);
            Dataset UpStairsdataset("./FilterData/filter_ADC.csv","./FilterData/filterori_IMU.csv",targetEMGUpStairspath,targetIMUUpStairspath,targetactionUpStairspath,targetgaitUpStairspath);
            UpStairsdataset.datasetcal(foot.GaitPhaseUpStairs,2);
        }
    }

    emit isDone();
}

void MyThreadALL::recordfile(QString path, std::vector<std::vector<int>> &gait,int nums){
    QFile label;
    label.setFileName(path);
    if (!label.open(QIODevice::Append))
    {

        return;
    }
    QTextStream out(&label);

    out<<"action"<<','<<nums<<"\n";
    for(int i=0;i<gait.size();++i){
        out<<","<<i<<',';
        for(int j=0;j<gait[i].size();++j){
            out<<gait[i][j]<<",";
        }
        out<<"\n";
    }
    label.close();
}

//=======================检测跳跃=======================
MyThreadJump::MyThreadJump(QObject *parent): QThread(parent){
    this->EMG_original_path = "./OriginalData/file_ADC.csv";
    this->EMG_filtered_path = "./FilterData/filter_ADC.csv";
    this->imu_original_path = "./OriginalData/file_IMU.csv";
    this->imu_filtered_path = "./FilterData/filter_IMU.csv";
    this->foot_original_path = "./OriginalData/file_FOOT.csv";
    this->foot_filtered_path = "./FilterData/filter_FOOT.csv";
    this->samp_path = "./FilterData/sampEN.csv";
    MyJump = new jump;//记录标签结果 记得刷新
    memset(MyJump, 0, sizeof(jump));
    action = new ALL_point;
    actionpre = new ALL_point;
    memset(action,0,sizeof(ALL_point));
    memset(actionpre,0,sizeof(ALL_point));
    HIGH = 1.05;
    LOW = 0.96;
}
MyThreadJump::~MyThreadJump(){
    delete action;
    delete actionpre;
    delete MyJump;
}
void MyThreadJump::findband(){
    QFile IMU;
    IMU.setFileName(imu_filtered_path);
    if (!IMU.open(QIODevice::ReadOnly))
    {

        return;
    }
    QTextStream in(&IMU);
    float acc = 0;
    for(int i=0;i<1000;++i){
        QString line = in.readLine();
        auto linelist = line.split(',');
        acc += linelist.at(1).toFloat();
    }
    HIGH = acc/1000.0 + 0.2;
    LOW = acc/1000.0 - 0.2;
}
/*===================检测活动段 得到所有活动段起始点============*/
int MyThreadJump::detect_ACT()
{
    int time_gap = 0;
    int res1 = ActiveSectionDetection(point,EMG_filtered_path,imu_filtered_path,samp_path,time_gap,HIGH,LOW);//检测活动段中跳跃部
    if(res1 < 0)
        return -1;
    qDebug()<<"检测跳跃的活动段带有IMU=========";
    for(int i=0;i<point.begin_count;++i){
        qDebug()<<"dafdsf"<<point.begin[i]<<" "<<point.end[i];
    }
    int res2 = AllActiveSectionDetection(point_2,EMG_filtered_path,imu_filtered_path,samp_path,HIGH,LOW);//检测所有的活动段
    if(res2 < 0)
        return -1;
    qDebug()<<"检测样本熵活动段IMU=========";
    for(int i=0;i<point_2.ACT_begin_count;++i){
        qDebug()<<point_2.ACT_begin[i]<<" "<<point_2.ACT_end[i];
    }
    return 0;
}
/*====================在活动段起始点之间检测下蹲的极限位置点=======*/
void MyThreadJump::detect_valley()
{
    qDebug()<<"检测跳跃活动段间的pitch";
    for (int i = 0; i < point.begin_count; i++)
    {
        int begin = point.begin[i]/ 10;
        int end = point.end[i] / 10;
        IMUPitchDetect Pitch(imu_filtered_path, begin, end);
        //Pitch.read_imu_data();
        Pitch.baselineIMU();

        if (Pitch.peak_count == 2) //有两次峰值-代表一次完美跳跃
        {
            //记录这次活动段的起始结束点
            this->MyJump->act_begin_point[this->MyJump->count] = point.begin[i];
            this->MyJump->act_end_point[this->MyJump->count] = point.end[i];
            //记录跳跃两次下蹲最低点
            this->MyJump->peak_begin_point[this->MyJump->count] = Pitch.peak[0];
            this->MyJump->peak_end_point[this->MyJump->count] = Pitch.peak[1];
            qDebug()<<point.begin[i]<<" "<<Pitch.peak[0]<<" "<<Pitch.peak[1]<<" "<<point.end[i];
            this->MyJump->count++;
        }
    }
}
/*==============在跳跃下蹲的点之间，检测跳跃空中的起始点============*/
void MyThreadJump::detect_rise()
{
     qDebug()<<"检测跳跃活动段间的foot";
    for (int i = 0; i < this->MyJump->count; ++i)
    {
        int peak1 = this->MyJump->peak_begin_point[i];
        int peak2 = this->MyJump->peak_end_point[i];
        int temp_foot_begin[10] = {0};
        int temp_foot_end[10] = {0};
        int number_foot = 0;
        //分别对全部4个足底压力通道进行检测
        for(int CH_FOOT=1;CH_FOOT<=4;CH_FOOT++)
        {
            JumpFootDetect foot(foot_filtered_path, CH_FOOT, peak1, peak2);
            foot.check();
            if (foot.rise_count >= 1)
            {
                for(int j=0;j<foot.rise_count;j++)
                {
                    if(foot.rise_end[j]-foot.rise_begin[j] >= 20)
                    {
                        temp_foot_begin[CH_FOOT-1] = foot.rise_begin[j];
                        temp_foot_end[CH_FOOT-1] = foot.rise_end[j];
                        break;

                    }
                }
            }
        }
        for(int k = 0; k<4; k++)
        {
            if(temp_foot_begin[k]-peak1 <0 || temp_foot_end[k]-peak2>0)
                continue;
            else
            {
                if(temp_foot_end[k]-temp_foot_begin[k] <= 300)//跳过空中间隔小于0.3s的数据点
                    continue;
                if(temp_foot_begin[k]-peak1 >= temp_foot_begin[number_foot]-peak1)
                    number_foot = k;
            }
        }
        this->MyJump->rise_begin_point[i] = temp_foot_begin[number_foot];
        this->MyJump->rise_end_point[i] = temp_foot_end[number_foot];
        qDebug()<<temp_foot_begin[number_foot]<<" "<<temp_foot_end[number_foot];
    }
}
/*=====================整合所有点===========================*/
void MyThreadJump::integrate_action()
{
    /*
     * MyJump 记录识别出的跳跃起始点等关键节点
     * point_2 记录所有计算出的活动段起始点，可能最后一次没有终点
     * ALL_point 记录完整的跳跃点
     */
    memset(action,0,sizeof(ALL_point));
    memset(actionpre,0,sizeof(ALL_point));
    for(int i=0; i<MyJump->count;++i)
    {
        for(int j=0; j <point_2.ACT_end_count; ++j)
        {
            if(point_2.ACT_begin[j]<=MyJump->act_begin_point[i] && \
                    point_2.ACT_end[j]+1000>= MyJump->act_end_point[i])//这个活动段包含跳跃段
            {
                //此时代表跳跃在这次活动段中
                action->action_begin[action->count] = point_2.ACT_begin[j];
                action->action_end[action->count] = point_2.ACT_end[j];
                action->jump_begin_point[action->count] = MyJump->act_begin_point[i];
                action->jump_end_point[action->count] = MyJump->act_end_point[i];
                action->peak_begin_point[action->count] =  MyJump->peak_begin_point[i];
                action->peak_end_point[action->count] =  MyJump->peak_end_point[i];
                action->rise_begin_point[action->count] =  MyJump->rise_begin_point[i];
                action->rise_end_point[action->count] =  MyJump->rise_end_point[i];
                if(point_2.ACT_end[j]<MyJump->act_end_point[i])
                {
                    action->jump_end_point[action->count] = point_2.ACT_end[j];
                }
                else if(point_2.ACT_end[j]-MyJump->act_end_point[i] >= 3000)//跳跃后，长时间没有离开活动段
                {
                    action->action_end[action->count] =MyJump->act_end_point[i]+3000;
                }
                if(MyJump->act_begin_point[i]-action->action_begin[action->count]>=2000)//一直处于前面的活动段，开始跳跃
                {
                    action->action_begin[action->count] = MyJump->act_begin_point[i]-1000;
                }
                action->count++;
                break;
            }
         }
    }
    //最后一次活动段没有结束点，这种情况下活动段开始点以后的跳跃，前面都找不到
    if(point_2.ACT_begin_count-point_2.ACT_end_count == 1) //最后一次活动段没有结束点
    {
        int last_begin_point = point_2.ACT_begin[point_2.ACT_begin_count-1];
        for(int i =0;i<MyJump->count;++i)
        {
            if(MyJump->act_begin_point[i] < last_begin_point)
                continue;//跳过最后一此活动段起点之前的跳跃段
            action->action_begin[action->count] =last_begin_point;
            action->action_end[action->count] = MyJump->act_end_point[i]+1000;//一直处于活动段
            action->jump_begin_point[action->count] = MyJump->act_begin_point[i];
            action->jump_end_point[action->count] = MyJump->act_end_point[i];
            action->peak_begin_point[action->count] =  MyJump->peak_begin_point[i];
            action->peak_end_point[action->count] =  MyJump->peak_end_point[i];
            action->rise_begin_point[action->count] =  MyJump->rise_begin_point[i];
            action->rise_end_point[action->count] =  MyJump->rise_end_point[i];

//            修正起点
            if(MyJump->act_begin_point[i]-action->action_begin[action->count]>=2000)//一直处于前面的活动段，开始跳跃
            {
                action->action_begin[action->count] = MyJump->act_begin_point[i]-1000;
            }
            action->count++;
        }
    }
    //进一步检测
    for(int i=0;i<this->action->count;++i){
        if(this->action->action_begin[i]<=0) continue;
        if(this->action->jump_begin_point[i]<=0) continue;
        if(this->action->peak_begin_point[i]<=0) continue;
        if(this->action->rise_begin_point[i]<=0) continue;
        if(this->action->rise_end_point[i]<=0) continue;
        if(this->action->peak_end_point[i]<=0) continue;
        if(this->action->jump_end_point[i]<=0) continue;
        if(this->action->action_end[i]<=0) continue;

        this->actionpre->action_begin[actionpre->count] = this->action->action_begin[i];
        this->actionpre->jump_begin_point[actionpre->count] = this->action->jump_begin_point[i];
        this->actionpre->peak_begin_point[actionpre->count] = this->action->peak_begin_point[i];
        this->actionpre->rise_begin_point[actionpre->count] = this->action->rise_begin_point[i];
        this->actionpre->rise_end_point[actionpre->count] = this->action->rise_end_point[i];
        this->actionpre->peak_end_point[actionpre->count] = this->action->peak_end_point[i];
        this->actionpre->jump_end_point[actionpre->count] = this->action->jump_end_point[i];
        this->actionpre->action_end[actionpre->count] = this->action->action_end[i];
        actionpre->count++;

    }
    qDebug()<<u8"完整识别结果=======================================";
    for (int i = 0; i < this->actionpre->count; ++i)
    {
        qDebug()<<QString(u8"    第 %1 次跳跃完整分段节点：").arg(i+1);
        qDebug()<<(QString(u8"%1,%2,%3,%4,%5,%6,%7,%8").arg(actionpre->action_begin[i])\
                                .arg(this->actionpre->jump_begin_point[i])\
                                .arg(actionpre->peak_begin_point[i])\
                                .arg(this->actionpre->rise_begin_point[i])\
                                .arg(this->actionpre->rise_end_point[i]).\
                                arg(actionpre->peak_end_point[i]).\
                                arg(this->actionpre->jump_end_point[i]).\
                                arg(actionpre->action_end[i]));
    }
}
void MyThreadJump::run()
{
//    qDebug()<<tr("mythread QThread::currentThreadId()==")<<QThread::currentThreadId();
    /*===================数据滤波================================*/
    //数据滤波
    //取第8组实验作为在线数据
    std::vector<int> lanqingfilenums = {1,2,4,5,6,9,10,11,12,13,14,15,16,17,18,19};
    std::vector<int> guoxiangfilenums = {1,2,3,4,5,6,7,9,10,11,12,13};
    std::vector<int> yufilenums = {1,2,3,4,5,6,7,9,10,11,12,13};
    std::vector<int> zhangfilenums = {1,2,3,4,5,6,7,9,10,11,12,13};
    std::vector<int> bofilenums = {1,2,3,4,5,6,7,8,9,10};
    //std::vector<int> bofilenums = {11};
    std::vector<int> filenumstest = {11};

    QString peoples[5] = {"yu","lan","guo","zhang","bo"};
    QString allpathemg[5] = {"jump_yu","jump_lan","jump_guo","jump_zhang","jump_bo"};
    std::vector<std::vector<int>> allfilenums;
    allfilenums.push_back(yufilenums);
    allfilenums.push_back(lanqingfilenums);
    allfilenums.push_back(guoxiangfilenums);
    allfilenums.push_back(zhangfilenums);
     allfilenums.push_back(bofilenums);

    for(int peo=4;peo<5;++peo){
        for(int filenum:allfilenums[peo]){
            QString pathEMG = QString("C:/Users/15492/Desktop/data/%1/%2/file_ADC.csv").arg(allpathemg[peo]).arg(filenum);
            QString pathIMU = QString("C:/Users/15492/Desktop/data/%1/%2/file_IMU.csv").arg(allpathemg[peo]).arg(filenum);
            QString pathFOOT = QString("C:/Users/15492/Desktop/data/%1/%2/file_FOOT.csv").arg(allpathemg[peo]).arg(filenum);
            qDebug()<<pathEMG<<Qt::endl;
            FilterData data(pathEMG,pathIMU,pathFOOT);
            int res1 = data.FilterEMGData();
            int res2 = data.FilterIMUData();
            int res3 = data.FilterFOOTData(50);
            if(res1<0){
                emit fileERROR("EMG filter error");
                return;
            }
            if(res2 < 0){
                emit fileERROR("IMU filter error");
                return;
            }
            if(res3 < 0){
                emit fileERROR("foot filter error");
                return;
            }

            memset(MyJump, 0, sizeof(jump));

            int res4 = this->detect_ACT();
            if(res4< 0)
            {
                emit fileERROR("打开文件出错");
                return;
            }
            this->detect_valley();
            this->detect_rise();
            this->integrate_action();
            //修正IMU峰值点
            FootDetect modifyIMU;
            //跳跃从 numsjump = {22} 修改为rightknee 最小值
            std::vector<int> numsjump = {22,34};
            gaitJump.clear();
            for (int i = 0; i < this->actionpre->count; ++i){
                int A = actionpre->action_begin[i];
                int B = actionpre->jump_begin_point[i];
                int C = actionpre->peak_begin_point[i];
                int D = actionpre->rise_begin_point[i];
                int E = actionpre->rise_end_point[i];
                int F = actionpre->peak_end_point[i];
                int G = actionpre->jump_end_point[i];
                int H = actionpre->action_end[i];
                int firstpeak = modifyIMU.FindImuPeak(B,D,numsjump);
                int secondpeak = modifyIMU.FindImuPeak(E,G,numsjump);
                std::vector<int> temp = {B,firstpeak,D,E,secondpeak,G};
                gaitJump.push_back(temp);
            }
            MyThreadALL too;


            QString targetlabelUpStairspath = QString("./FilterData/%1/Jumplabel.csv").arg(peoples[peo]);
            QString targetEMGUpStairspath = QString("./Dataset/%1/JumpEMGfeature.csv").arg(peoples[peo]);
            QString targetIMUUpStairspath = QString("./Dataset/%1/JumpIMUfeature.csv").arg(peoples[peo]);
            QString targetactionUpStairspath = QString("./Dataset/%1/Jumpaction.csv").arg(peoples[peo]);
            QString targetgaitUpStairspath = QString("./Dataset/%1/Jumpgaitphase.csv").arg(peoples[peo]);

            too.recordfile(targetlabelUpStairspath,gaitJump,filenum);
            Dataset Jumpdataset("./FilterData/filter_ADC.csv","./FilterData/filterori_IMU.csv",targetEMGUpStairspath,targetIMUUpStairspath,targetactionUpStairspath,targetgaitUpStairspath);
            Jumpdataset.datasetcal(gaitJump,5);
        }
    }
    emit isDone();

}

//***********************************上下坡***********************************************
MyThreadLabelUpHill::MyThreadLabelUpHill(QObject *parent): QThread(parent)
{
    action.push_back(0);
}
void MyThreadLabelUpHill::SetAction(int n){
    action.push_back(n);
}
MyThreadLabelUpHill::~MyThreadLabelUpHill(){}
void MyThreadLabelUpHill::run(){
    //数据滤波 3 11 12 有问题
    //取第10组实验作为在线数据
    std::vector<int> filenumsyu = {1,2,4,5,6,7,8,9,11,12,13,14,15};
    std::vector<int> filenumslan = {1,2,3,4,5,6,7,8,9,11,12};
    std::vector<int> filenumsguo = {1,2,4,5,6,7,8,9,11,12,13};
    std::vector<int> filenumszhang = {1,2,3,4,5,6,7,8};
    std::vector<int> filenumsbo = {1,2,3,4,5,6,7,8,9,10};
    //std::vector<int> filenumsbo = {11,12};
    std::vector<int> filenums = {6,10};

    QString peoples[5] = {"yu","lan","guo","zhang","bo"};
    QString allpathemg[5] = {"yu","lan","guo","zhang","bo"};
    std::vector<std::vector<int>> allfilenums;
    allfilenums.push_back(filenumsyu);
    allfilenums.push_back(filenumslan);
    allfilenums.push_back(filenumsguo);
    allfilenums.push_back(filenumszhang);
    allfilenums.push_back(filenumsbo);

    for(int peo=4;peo<5;++peo){
        for(int filenum : allfilenums[peo]){
            QString pathEMG = QString("C:/Users/15492/Desktop/data/updownhill/%1/%2/file_ADC.csv").arg(allpathemg[peo]).arg(filenum);
            QString pathIMU = QString("C:/Users/15492/Desktop/data/updownhill/%1/%2/file_IMU.csv").arg(allpathemg[peo]).arg(filenum);
            QString pathFOOT = QString("C:/Users/15492/Desktop/data/updownhill/%1/%2/file_FOOT.csv").arg(allpathemg[peo]).arg(filenum);
            qDebug()<<pathEMG<<Qt::endl;
            FilterData data(pathEMG,pathIMU,pathFOOT);
            int res1 = data.FilterEMGData();
            int res2 = data.FilterIMUData();
            int res3 = data.FilterFOOTData(50);
            if(res1<0){
                emit fileERROR("EMG filter error");
                return;
            }
            if(res2 < 0){
                emit fileERROR("IMU filter error");
                return;
            }
            if(res3 < 0){
                emit fileERROR("foot filter error");
                return;
            }
            foot.FindAirFootPoint(1,foot.ch1); //寻找第一通道的滞空足底，即右脚脚掌
            foot.FindAirFootPoint(2,foot.ch2); //寻找第二通道的滞空足底，即右脚脚后跟
            foot.FindHeelToe(); //整合右脚的一次步态相位的4个点

            //上坡
            //上坡不变 还是用leftshank
            std::vector<int> actionuphill = {0,2};
            std::vector<int> numsuphill={70}; //使用IMU第几列的数据
            foot.FindUpHillGaitphasePoint(actionuphill,numsuphill);

            QString targetlabelUpStairspath = QString("./FilterData/%1/UpHilllabel.csv").arg(peoples[peo]);
            QString targetEMGUpStairspath = QString("./Dataset/%1/UpHillEMGfeature.csv").arg(peoples[peo]);
            QString targetIMUUpStairspath = QString("./Dataset/%1/UpHillIMUfeature.csv").arg(peoples[peo]);
            QString targetactionUpStairspath = QString("./Dataset/%1/UpHillaction.csv").arg(peoples[peo]);
            QString targetgaitUpStairspath = QString("./Dataset/%1/UpHillgaitphase.csv").arg(peoples[peo]);



            recordfile(targetlabelUpStairspath,foot.GaitPhaseUpHill,filenum);
            for(int i=0;i<foot.GaitPhaseUpHill.size();++i){
                qDebug()<<"uphill"<<" "<<foot.GaitPhaseUpHill[i][0]<<" "<<foot.GaitPhaseUpHill[i][1]<<" "<<foot.GaitPhaseUpHill[i][2]<<" "<<foot.GaitPhaseUpHill[i][3]<<" "<<foot.GaitPhaseUpHill[i][4]<<" "<<foot.GaitPhaseUpHill[i][5]<<" ";
            }
            Dataset UpHilldataset("./FilterData/filter_ADC.csv","./FilterData/filterori_IMU.csv",targetEMGUpStairspath,targetIMUUpStairspath,targetactionUpStairspath,targetgaitUpStairspath);
            UpHilldataset.datasetUpDownHillcal(foot.GaitPhaseUpHill,0);

            //下坡下坡numsdownhill={70} 修改为rightshank 34
            std::vector<int> actiondownhill = {1,3};
            std::vector<int> numsdownhill={34}; //使用IMU第几列的数据
            foot.FindDownHillGaitphasePoint(actiondownhill,numsdownhill);

            QString targetlabeldownStairspath = QString("./FilterData/%1/DownHilllabel.csv").arg(peoples[peo]);
            QString targetEMGdownStairspath = QString("./Dataset/%1/DownHillEMGfeature.csv").arg(peoples[peo]);
            QString targetIMUdownStairspath = QString("./Dataset/%1/DownHillIMUfeature.csv").arg(peoples[peo]);
            QString targetactiodownStairspath = QString("./Dataset/%1/DownHillaction.csv").arg(peoples[peo]);
            QString targetgaitdownStairspath = QString("./Dataset/%1/DownHillgaitphase.csv").arg(peoples[peo]);


            recordfile(targetlabeldownStairspath,foot.GaitPhaseDownHill,filenum);
            for(int i=0;i<foot.GaitPhaseDownHill.size();++i){
                qDebug()<<"downhill"<<" "<<foot.GaitPhaseDownHill[i][0]<<" "<<foot.GaitPhaseDownHill[i][1]<<" "<<foot.GaitPhaseDownHill[i][2]<<" "<<foot.GaitPhaseDownHill[i][3]<<" "<<foot.GaitPhaseDownHill[i][4]<<" "<<foot.GaitPhaseDownHill[i][5]<<" ";
            }
            Dataset DownHilldataset("./FilterData/filter_ADC.csv","./FilterData/filterori_IMU.csv",targetEMGdownStairspath,targetIMUdownStairspath,targetactiodownStairspath,targetgaitdownStairspath);
            DownHilldataset.datasetUpDownHillcal(foot.GaitPhaseDownHill,1);
        }
    }

    emit isDone();
}
void MyThreadLabelUpHill::recordfile(QString path, std::vector<std::vector<int>> &gait,int nums){
    QFile label;
    label.setFileName(path);
    if (!label.open(QIODevice::Append))
    {

        return;
    }
    QTextStream out(&label);

    out<<"action"<<','<<nums<<"\n";
    for(int i=0;i<gait.size();++i){
        out<<","<<i<<',';
        for(int j=0;j<gait[i].size();++j){
            out<<gait[i][j]<<",";
        }
        out<<"\n";
    }
    label.close();
}
