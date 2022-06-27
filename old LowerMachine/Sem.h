#pragma once

//创建一些信号
int Sem_creat_GLB();//����ȫ�ֱ������ź���
int Sem_creat_ADC();//ADC����һ�������ź���
int Sem_creat_IMU(); //����һ�������ź�����
int Sem_creat_FOOT();//���ѹ���ź���
int Sem_creat_DATA(); //�������ݰ��ṹ���ź�����
int Sem_creat_ACT();
int Sem_creat_TCP();
int Sem_creat_TIMER();
bool Sem_P(int semid);//�ź���P����
void Sem_V(int semid);//�ź���V����
void Sem_set(int semid);//�ź���ȫ����Ϊ1��
void Sem_remove(int semid);
