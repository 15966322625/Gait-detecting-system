/******************************************************
*			TCP �����
*
*********************************************************/
#pragma once
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include "config.h"
#include "global.h"

#define TCP_port	3333         //设置端口号
typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

typedef struct {		//	TCP ���ݽṹ	
	char Data[100];
} TCP_Data_t;

class tcp_TCPClass
{
public:
	tcp_TCPClass();
	~tcp_TCPClass();
public:
	int init();//初始化TCP进程
	int send(char *src,int coun);//发送数据
	bool c_flag = false;//true:	成功建立TCP连接
private:
	int socket_fd;	//设备标识符
	int TCP_fd;
	//FILE* print_off;
	bool s_flag = false;//true:	进行TCP数据发送
	sockaddr_in_t server_addr, client_addr;
};

//extern bool TCP_bkup(TCP_Data_t *dest, TCP_Data_t *src);









