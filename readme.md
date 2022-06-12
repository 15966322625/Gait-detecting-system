# Gait detecting system
基于Cpp11实现的步态检测人机交互系统设计，意在为一套可穿戴式的外骨骼设备控制提供步态检测识别结果的控制指令，实现外骨骼设备助力、辅助行走等现实应用。
## 整体框架
- 整个项目的实现分为上位机Gra3_0和下位机EMG两部分。
- 上位机主要实现界面设计方面的工作，实现和实验人员的交互作用，效果展示等作用。
- 上位机主要完成的工作是对六个步态划分阶段以及每个阶段的五个状态的结果展示。
- 上位机主要会展示连接状态、识别时间以及各个步态的划分结果，还包括数据标注、模型移植等方面的按钮。
- 界面主要是在Qt环境下基于C++多线程高并发等功能实现的人机交互系统。

## 主要实现技术
- Linux下常用操作、cmake文件编写等。
- Qt关于信号和槽的设计。
- 数据检测算法的实现。
- 数据滤波算法的实现。
- 自定义工具函数类、网络类、多线程类等多个功能类。
- 实现BP网络模型的算法移植。
- 局域网下实现客户端和服务端连接。
- 多线程实现六个步态数据集的循环采集。
- 实现日志系统的设计，监视程序及设备运行状态，记录出现的问题。
- 用vector容器实现了一个可自动扩容的缓冲区。
- 利用单例模式实现了一个简单的线程池，减少了线程创建与销毁的开销

## 环境要求
- Linux-RaspBerry Pi
- C++11
- 设备及软件要求：树莓派 AD控制板 计算机 Qt编程软件 Xshell Xftp

## 整体程序框架
### 上位机(upptercomputer)整体代码目录树
```
.
├── bin
│   └── uppercomputer 可执行文件
├── build
│   └── Makefile
├── code             源代码
│   ├── ActiveSection      活动段检测部分
│   ├── config       配置文件
│   ├── Dataset     数据集制作部分
│   ├── filterdata       数据滤波部分
│   ├── FootDetect        根据足底开关信号检测足部事件处理代码
│   ├── main.cpp     主函数
│   ├── IMUPitchDetect      IMU姿态传感器数据特殊点检测模块
│   ├── JumpFootDetect  跳跃动作滞空检测阶段
│   ├── mainwindow   主界面
│   ├── MyDataStruct  制作自己的数据集阶段
│   ├── mypushbutton  Qt里的按钮和槽
│   ├── MyThread  自定义线程，完成数据标注，制作数据集工作。
│   ├── Mytool  工具函数
│   └── tcpclient       自定义tcp连接类：上位机充当和下位机建立连接的上位机
├── log              日志目录
├── Makefile
├── README.md
└── resources        静态资源
```

### 下位机(lowercomputer)整体代码目录树
```
.
├── bin
│   └── lowercomputer 可执行文件
├── build
│   └── Makefile
├── code             源代码
│   ├── CMakeLists cmake文件 
│   ├── BP_DATA、BPNet、class_Dense 手写的BP网络模型结构  
│   ├── ADC_OL sEMG信号采集芯片AD7606初始化
│   ├── CAN_class      can通信类
│   ├── FootDetect        足底压力检测模块
│   ├── config.h     配置文件
│   ├── data_ADC_main sEMG信号采集进程
│   ├── data_Foot_main  足底开关信号采集进程
│   ├── IMU_data   IMU传感器初始化类   
│   ├── data_IMU_main  IMU信号采集进程  
│   ├── main_data 数据同步类  
│   ├── data_WIN  数据窗类
│   ├── dsp_ACT  判断活动段类
│   ├── dsp_RCGClass   调用行为模型类   
│   ├── filt_ADC  数据滤波  
│   ├── global 全局变量 定义结构体  
│   ├── main_dsp  处理数据 判断活动段
│   ├── dsp_RCG_main  处理数据，进行行为识别
│   ├── main_tcp 建立TCP\IP连接通信 下位机为服务端 上位机为客户端
│   └── main       程序入口
├── log              日志目录
├── Makefile
├── README.md
└── resources        静态资源
```
