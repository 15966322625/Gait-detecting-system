#include "filterdata.h"
#include <QTextStream>
#include <QMessageBox>
#include <QString>
#include <vector>
#include <QDebug>
#include "MyTool.h"
#include "Configure.h"

//实现对三种数据的总体滤波处理
FilterData::FilterData(QString EMGpath, QString IMUpath,QString FOOTpath)//参数是三种数据的文件路径
{
    EMG = new QFile;
    this->EMGpath = EMGpath;
    EMG->setFileName(EMGpath);
    IMU = new QFile;
    this->IMUpath = IMUpath;
    IMU->setFileName(IMUpath);
    FOOT = new QFile;
    this->FOOTpath = FOOTpath;
    FOOT->setFileName(FOOTpath);
}
FilterData::~FilterData()
{

}
int FilterData::FilterEMGData()
{
    int Tnow = 1;
    float temp_adc[8] = { 0 };
    float filter_adc[8] = { 0 };
    if (!EMG->open(QIODevice::ReadOnly))
    {
        return -1;
    }
    QTextStream in(EMG);
    QFile adc_after;
    adc_after.setFileName("./FilterData/filter_ADC.csv");//滤波之后的文件名称
    if (!adc_after.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream out(&adc_after);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        auto ll = line.split(',');
        for (int i = 1; i < ll.length(); i++)
        {
            temp_adc[i - 1] = ll.at(i).toFloat();
        }
        Tnow = ll.at(0).toInt();
        ADC_filter(temp_adc, filter_adc);
        out << Tnow << ',';
        for (int i = 0; i < 7; i++)
        {
            out <<filter_adc[i] << ',';
        }
        out << filter_adc[7] << Qt::endl;
    }
    EMG->close();
    adc_after.close();
    return 0;
}
int FilterData::FilterIMUData()
{
    int count = 1;
    QTextStream in(IMU);
    if (!IMU->open(QIODevice::ReadOnly))
    {
        return -1;
    }
    in.readLine();//第一行不要

    //滤波文件，用于标注
    QFile IMU_after;
    IMU_after.setFileName("./FilterData/filter_IMU.csv");
    if (!IMU_after.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream out(&IMU_after);

    //不滤波，用于制作数据集

    QFile IMU_after_ori;
    IMU_after_ori.setFileName("./FilterData/filterori_IMU.csv");
    if (!IMU_after_ori.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream outori(&IMU_after_ori);

    float IMU_before_data[8][12] = { 0 };
    float IMU_after_data[8][12] = { 0 };
    while (!in.atEnd())
    {
        QString line = in.readLine();
        auto linelist = line.split(',');
        for (int i = 0; i < linelist.length() - 1; i++)
        {
            IMU_before_data[i / 12][i % 12] = linelist.at(i + 1).toFloat();
        }

        IMU_filter(IMU_before_data, IMU_after_data);

        count = linelist.at(0).toInt();
        count /= 10;

        out << count * 10 << ',';
        for (int i = 0; i < (7 * 12)-1; i++)
        {
            out << IMU_after_data[i / 12][i % 12] << ',';
        }
        out << IMU_after_data[7-1][11] << Qt::endl;

        outori << count * 10 << ',';
        for (int i = 0; i < (7 * 12)-1; i++)
        {
            outori << IMU_before_data[i / 12][i % 12] << ',';
        }
        outori << IMU_before_data[7-1][11] << Qt::endl;


        count++;

    }

    IMU->close();
    IMU_after.close();
    IMU_after_ori.close();

    return 0;
}

//数据修正
static int modify2(QString pathori,QString pathtarget){
    QFile foot_afterch;
    foot_afterch.setFileName(pathori);
    if (!foot_afterch.open(QIODevice::ReadOnly))
    {
        return -2;
    }
    QTextStream chin(&foot_afterch);

    QFile foot_after;
    foot_after.setFileName(pathtarget);
    if (!foot_after.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream out(&foot_after);
    std::vector<std::pair<int,int>> ch;
    ch.reserve(10000);
    ch.clear();
    while(!chin.atEnd()){
        QString line = chin.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(1).toInt();//足底开关当前值
        if(sw == 0){
            if(!ch.empty())
            {
                int n = ch.size();
                int value = 1;
                if(n>=1500){
                    value = 0;
                }
                for(int j = 0;j<n;++j){
                    out<<ch[j].first<<','<<value<<Qt::endl;
                }
                ch.clear();
            }
            out<<Tnow<<','<<sw<<Qt::endl;
        }
        else{
            ch.push_back({Tnow,sw});
        }
    }
    //如果以1结尾
    if(!ch.empty())
    {
        int n = ch.size();
        int value = 1;
        if(n>=1500){
            value = 0;
        }
        for(int j = 0;j<n;++j){
            out<<ch[j].first<<','<<value<<Qt::endl;
        }
        ch.clear();
    }
    foot_after.close();
    foot_afterch.close();
}
int FilterData::FilterFOOTData(int keep)
{
    std::vector<std::pair<int,int>> ch1;
    ch1.reserve(10000);
    ch1.clear();
    if (!FOOT->open(QIODevice::ReadOnly))
    {
        return -1;
    }
    QTextStream in(FOOT);
    QFile foot_after;
    foot_after.setFileName("./FilterData/filter_FOOT.csv");
    if (!foot_after.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream out(&foot_after);


    QFile foot_afterch1;
    foot_afterch1.setFileName("./FilterData/filter_FOOTch1.csv");
    if (!foot_afterch1.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream outch1(&foot_afterch1);

    QFile foot_afterch2;
    foot_afterch2.setFileName("./FilterData/filter_FOOTch2.csv");
    if (!foot_afterch2.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream outch2(&foot_afterch2);

    QFile foot_afterch3;
    foot_afterch3.setFileName("./FilterData/filter_FOOTch3.csv");
    if (!foot_afterch3.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream outch3(&foot_afterch3);

    QFile foot_afterch4;
    foot_afterch4.setFileName("./FilterData/filter_FOOTch4.csv");
    if (!foot_afterch4.open(QIODevice::WriteOnly))
    {
        return -2;
    }
    QTextStream outch4(&foot_afterch4);
//*********************************ch1************************************
    int CH = 1;
    int long_time = 0;//上一次长时间持续的值
    int count = 0;
    while(!in.atEnd()){
        QString line = in.readLine();
        ++count;
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(CH).toInt();//足底开关当前值
        if(ch1.empty())
        {
            ch1.push_back({Tnow,sw});
        }
        else
        {
            int N = ch1.size();
            int last_sw = ch1[N-1].second;
            if(sw == last_sw)
            {
                ch1.push_back({Tnow,sw});
            }
            else
            {
                int value = last_sw;
                if(N <= keep)
                {
                    value = long_time;
                }
                for(int i=0;i<N;++i){
                    outch1<<ch1[i].first<<','<<value<<Qt::endl;
                }

                ch1.clear();
                long_time = value;
                ch1.push_back({Tnow,sw});
            }
        }
    }
    if(!ch1.empty())
    {
        int N = ch1.size();
        int last_sw = ch1[N-1].second;
        int value = last_sw;
        //sw != last_sw
        if(N <= keep)
        {
            value = long_time;
        }
        for(int i=0;i<N;++i){
            outch1<<ch1[i].first<<','<<value<<Qt::endl;
        }
        ch1.clear();
    }
//*************ch2************************
    in.seek(0);
    long_time = 0;
    CH = 2;
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(CH).toInt();//足底开关当前值
        if(ch1.empty())
        {
            ch1.push_back({Tnow,sw});
        }
        else
        {
            int N = ch1.size();
            int last_sw = ch1[N-1].second;
            if(sw == last_sw)
            {
                ch1.push_back({Tnow,sw});
            }
            else{
                int value = last_sw;
                //sw != last_sw
                if(N <= keep)
                {
                    value = long_time;
                }
                for(int i=0;i<N;++i){
                    outch2<<ch1[i].first<<','<<value<<Qt::endl;
                }
                ch1.clear();
                long_time = value;
                ch1.push_back({Tnow,sw});
            }
        }
    }
    if(!ch1.empty())
    {
        int N = ch1.size();
        int last_sw = ch1[N-1].second;
        int value = last_sw;
        //sw != last_sw
        if(N <= keep)
        {
            value = long_time;
        }
        for(int i=0;i<N;++i){
            outch2<<ch1[i].first<<','<<value<<Qt::endl;
        }
        ch1.clear();
    }
//***************ch3***********************
    in.seek(0);
    long_time = 0;
    CH = 3;
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(CH).toInt();//足底开关当前值
        if(ch1.empty())
        {
            ch1.push_back({Tnow,sw});
        }
        else
        {
            int N = ch1.size();
            int last_sw = ch1[N-1].second;
            if(sw == last_sw)
            {
                ch1.push_back({Tnow,sw});
            }
            else{
                int value = last_sw;
                //sw != last_sw
                if(N <= keep)
                {
                    value = long_time;
                }
                for(int i=0;i<N;++i){
                    outch3<<ch1[i].first<<','<<value<<Qt::endl;
                }
                ch1.clear();
                long_time = value;
                ch1.push_back({Tnow,sw});
            }
        }
    }
    if(!ch1.empty())
    {
        int N = ch1.size();
        int last_sw = ch1[N-1].second;
        int value = last_sw;
        //sw != last_sw
        if(N <= keep)
        {
            value = long_time;
        }
        for(int i=0;i<N;++i){
            outch3<<ch1[i].first<<','<<value<<Qt::endl;
        }
        ch1.clear();
    }
//************************ch4***************************
    in.seek(0);
    long_time = 0;
    CH = 4;
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(CH).toInt();//足底开关当前值
        if(ch1.empty())
        {
            ch1.push_back({Tnow,sw});
        }
        else
        {
            int N = ch1.size();
            int last_sw = ch1[N-1].second;
            if(sw == last_sw)
            {
                ch1.push_back({Tnow,sw});
            }
            else{
                int value = last_sw;
                //sw != last_sw
                if(N <= keep)
                {
                    value = long_time;
                }
                for(int i=0;i<N;++i){
                    outch4<<ch1[i].first<<','<<value<<Qt::endl;
                }
                ch1.clear();
                long_time = value;
                ch1.push_back({Tnow,sw});
            }
        }
    }
    if(!ch1.empty())
    {
        int N = ch1.size();
        int last_sw = ch1[N-1].second;
        int value = last_sw;
        //sw != last_sw
        if(N <= keep)
        {
            value = long_time;
        }
        for(int i=0;i<N;++i){
            outch4<<ch1[i].first<<','<<value<<Qt::endl;
        }
        ch1.clear();
    }
    FOOT->close();

    foot_afterch1.close();
    foot_afterch2.close();
    foot_afterch3.close();
    foot_afterch4.close();

//    if (!foot_afterch1.open(QIODevice::ReadOnly))
//    {
//        return -1;
//    }
//    if (!foot_afterch2.open(QIODevice::ReadOnly))
//    {
//        return -1;
//    }
//    if (!foot_afterch3.open(QIODevice::ReadOnly))
//    {
//        return -1;
//    }
//    if (!foot_afterch4.open(QIODevice::ReadOnly))
//    {
//        return -1;
//    }
    //***************进一步修正每一个通道 将长时间保持为离开地面的点修正*****************************
    modify2("./FilterData/filter_FOOTch1.csv","./FilterData/filter_FOOTM2ch1.csv");
    modify2("./FilterData/filter_FOOTch2.csv","./FilterData/filter_FOOTM2ch2.csv");
    modify2("./FilterData/filter_FOOTch3.csv","./FilterData/filter_FOOTM2ch3.csv");
    modify2("./FilterData/filter_FOOTch4.csv","./FilterData/filter_FOOTM2ch4.csv");
    //******************合并结果*********************************************************
    foot_afterch1.setFileName("./FilterData/filter_FOOTM2ch1.csv");
    if (!foot_afterch1.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    foot_afterch2.setFileName("./FilterData/filter_FOOTM2ch2.csv");
    if (!foot_afterch2.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    foot_afterch3.setFileName("./FilterData/filter_FOOTM2ch3.csv");
    if (!foot_afterch3.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    foot_afterch4.setFileName("./FilterData/filter_FOOTM2ch4.csv");
    if (!foot_afterch4.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    outch1.setDevice(&foot_afterch1);
    outch2.setDevice(&foot_afterch2);
    outch3.setDevice(&foot_afterch3);
    outch4.setDevice(&foot_afterch4);
    outch1.seek(0);
    outch2.seek(0);
    outch3.seek(0);
    outch4.seek(0);
    while(!outch1.atEnd() && !outch2.atEnd() && !outch3.atEnd() && !outch4.atEnd())
    {
        QString line = outch1.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();
        int sw1 = linelist.at(1).toInt();
        int sw2 = outch2.readLine().split(',').at(1).toInt();
        int sw3 = outch3.readLine().split(',').at(1).toInt();
        int sw4 = outch4.readLine().split(',').at(1).toInt();
        out<<Tnow<<','<<sw1<<','<<sw2<<','<<sw3<<','<<sw4<<Qt::endl;
    }

    foot_afterch1.close();
    foot_afterch2.close();
    foot_afterch3.close();
    foot_afterch4.close();
    foot_after.close();
    qDebug()<<"完成足底开关滤波"<<"共读到行数"<<count<<"\n";
    return 0;
}

