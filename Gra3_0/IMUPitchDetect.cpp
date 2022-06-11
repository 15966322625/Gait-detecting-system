#include "IMUPitchDetect.h"
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

IMUPitchDetect::IMUPitchDetect(const QString& path,int begin,int end)
{
    IMU_file = new QFile;
    IMU_file->setFileName(path);
    this->begin = begin;
    this->end = end;
    peak_count = 0;
    valley_count = 0;
    memset(peak, 0, sizeof(peak));
    memset(pitch_mean, 0, sizeof(pitch_mean));
    memset(&record, 0, sizeof(record_pitch_max));
    this->record.flag = SEEK_BIGER;
    this->record.pitch_record = -40.0;
}
IMUPitchDetect::~IMUPitchDetect()
{
    delete IMU_file;
}
void IMUPitchDetect::baselineIMU(){
    int count = this->begin;
    float sum = 0.0f;
    int tag = 0;
    if (!IMU_file->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, "警告", "IMU数据文件打开出错", QMessageBox::Yes);
        exit(0);
    }
    QTextStream imu_in(IMU_file);
    for (int i = 0; i < this->begin; i++)//EMG数据和IMU数据同步（进来的begin=（2001/10）200--（2001~2010为adc的起点对应的imu为第210）
    {
        if (imu_in.atEnd())
        {
            return;
        }
        imu_in.readLine();
    }
    float baselinepitch = -60.0;
    while (count <= this->end && (!imu_in.atEnd())){
        count++;
        QString temp = imu_in.readLine();
        auto temp_list = temp.split(',');
        int Tnow = temp_list.at(0).toInt();
        int value = temp_list.at(NUMBER_IMU).toFloat();
        //第一个点为60° 第二个点为40°
        if(value>baselinepitch){
            //在基线-40°上找最大值
            float last_value = -60.0;
            int last_tnow = -1;
            while(count <= this->end && (!imu_in.atEnd())){
                count++;
                temp = imu_in.readLine();
                temp_list = temp.split(',');
                Tnow = temp_list.at(0).toInt();
                value = temp_list.at(NUMBER_IMU).toFloat();
                if(value>last_value){
                    last_tnow = Tnow;
                    last_value = value;
                }
                if(value<baselinepitch) break;
            }
            if(last_tnow != -1){
                //找到第一个点，开始找第二个点
                peak[this->peak_count] = last_tnow;//以EMG数据的点为记录，记录为起点
                this->peak_count++;
                baselinepitch=-40.0;
                qDebug()<<"new jian ce"<<last_tnow;
            }
        }
    }
}
void IMUPitchDetect::read_imu_data()
{
    int count = this->begin;
    float sum = 0.0f;
    int tag = 0;
    if (!IMU_file->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, "警告", "IMU数据文件打开出错", QMessageBox::Yes);
        exit(0);
    }
    QTextStream imu_in(IMU_file);
    for (int i = 0; i < this->begin; i++)//EMG数据和IMU数据同步（进来的begin=（2001/10）200--（2001~2010为adc的起点对应的imu为第210）
    {
        if (imu_in.atEnd())
        {
            return;
        }
        imu_in.readLine();
    }
    while (count <= this->end && (!imu_in.atEnd())) //每4个求一个平均值
    {
        QString temp = imu_in.readLine();
        auto temp_list = temp.split(',');
        static pitch_node temp_pitch[MEAN_WIDTH] = { 0 };
        temp_pitch[tag].point = temp_list.at(0).toInt();
        temp_pitch[tag].value = temp_list.at(NUMBER_IMU).toFloat();//(使用第三个IMU—大腿上的)
        count++;
        tag++;
        if (tag >= MEAN_WIDTH)
        {
            tag = 0;
            float temp_max = temp_pitch[0].value;
            int temp_point = temp_pitch[0].point;
            sum = temp_pitch[0].value;
            if(MEAN_WIDTH != 1)
            {
                for (int i = 1; i < MEAN_WIDTH; ++i) //找到这几个求平均值中的最大的pitch点,作为这次几个点平均值的点（主要找峰值点）
                {
                    sum += temp_pitch[i].value;
                    if (float(temp_pitch[i].value - temp_max) >= EPSILON)
                    {
                        temp_max = temp_pitch[i].value;
                        temp_point = temp_pitch[i].point;
                    }
                }
                sum = sum / float(MEAN_WIDTH);
            }
            this->pitch_mean_appen(temp_point,sum);
            sum = 0.0f;
        }
    }
    IMU_file->close();
}
void IMUPitchDetect::move_window()
{
    int i = WIDTH / 2;
    for (int j = 0; j < i; j++)
    {
        this->pitch_mean[j].value = this->pitch_mean[j + i].value;
        this->pitch_mean[j].point = this->pitch_mean[j + i].point;
    }
}
void IMUPitchDetect::pitch_mean_appen(int point,float acc_mean)
{
    static int count = 0;
    this->pitch_mean[count].point = point;
    this->pitch_mean[count].value = acc_mean;//这里可以改一移动窗的步长
    count++;
    if (count >= WIDTH)//窗口有10个数据 进行判断
    {
        switch (this->record.flag)
        {
        case SEEK_BIGER:
            if (this->peak_judge(this->pitch_mean))
            {
                peak[this->peak_count] = this->record.point-9;//以EMG数据的点为记录，记录为起点
                this->peak_count++;
            }
            break;
        case SEEK_SMALLER:
            if (this->valley_judge(this->pitch_mean))
            {
                valley[this->valley_count] = this->record.point-9;
                this->valley_count++;
            }
            break;
        default:break;
        }
        /*this->move_window();
        count = WIDTH/2;*/
        count = 0;
    }
}
void IMUPitchDetect::set_begin_end(int begin_point, int end_point)
{
    this->begin = begin_point;
    this->end = end_point;
    peak_count = 0;
    valley_count = 0;
    this->record.flag = SEEK_BIGER;
    this->record.pitch_record = -100.0;
}
bool IMUPitchDetect::peak_judge(pitch_node* data)
{
    float temp_max = 0.0f;
    int temp_count = 0;
    static int count = 0;
    if (data[WIDTH-1].value < (-40.0f)) //表示这段数据窗里不可能含有峰值点（-60到10）
    {
        this->record.pitch_record = data[WIDTH-1].value;
        this->record.point = data[WIDTH-1].point;
        return false;
    }
    temp_max = data[0].value;
    temp_count = data[0].point;
    for (int i = 1; i < WIDTH; i++)//找出目前窗的最大值 和暂时保存的比较
    {
        if (float(data[i].value - temp_max) >= EPSILON)
        {
            temp_max = data[i].value;
            temp_count = data[i].point;
        }
    }
    //和上一次窗的结果比较
    if (float(temp_max - record.pitch_record) >= EPSILON) //pitch 还是逐渐增大 更新record
    {
        count = 0;
        this->record.pitch_record = temp_max;
        this->record.point = temp_count;
    }
    else //发现目前窗的最大值pitch突然变小，（可以考虑连续两次对比记录的record变小）
    {
        count++;
        if (count >= MAX_COUNT) //连续两次下降 此时的record记录的为峰值点 变为寻找谷底
        {
            this->record.pitch_record = temp_max;
            this->record.point = temp_count;
            this->record.flag = SEEK_SMALLER;
            return true;
        }
    }
    return false;
}
bool IMUPitchDetect::valley_judge(pitch_node* data) //逐渐减小 遇到突然变大，此时record为峰谷
{
    float temp_min = data[0].value;
    int temp_point = data[0].point;
    static int count = 0;
    for (int i = 1; i < WIDTH; i++)//找到一个窗的最小点
    {
        if (float(temp_min - data[i].value) >= EPSILON)
        {
            temp_min = data[i].value;
            temp_point = data[i].point;
        }
    }
    if (float(this->record.pitch_record - temp_min) >= EPSILON)
    {
        count = 0;
        this->record.pitch_record = temp_min;
        this->record.point = temp_point;
    }
    else
    {
        count++;
        if (count >= MAX_COUNT) //连续两次上升 此时的record记录的为峰谷点 变为寻找峰值点
        {
            this->record.pitch_record = temp_min;
            this->record.point = temp_point;
            this->record.flag = SEEK_BIGER;
            return true;
        }
    }
    return false;
}
