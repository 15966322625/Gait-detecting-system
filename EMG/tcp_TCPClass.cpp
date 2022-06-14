#include "tcp_TCPClass.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>
#include <pthread.h>
#define TCP_MAX_Listen 5


tcp_TCPClass::tcp_TCPClass()
{
	
}
tcp_TCPClass::~tcp_TCPClass()
{
	//fclose(print_off);
	close(TCP_fd);
}

//��ʼ��������TCP�������,ֱ�����ڿͻ�����������������Ӻ���з���
int tcp_TCPClass::init()
{
	/*	1.服务器端建立sockfd描述符	*/
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{	// AF_INET:IPV4	SOCK_STREAM:TCP	
		fprintf(stderr, "Socket error:%s\n\a", strerror(errno));
		return -1;
	}
	/*	2.服务器端填充 sockaddr结构	*/
	bzero(&server_addr, sizeof(sockaddr_in_t));		//初始化 置为0
	server_addr.sin_family = AF_INET;		//Internet IPV4
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//将本机上的long数据转化为网络上的long数据 服务器程序能运行在任何IP的主机上
	//INADDR_ANY 表示主机可以是任意IP地址 即服务器程序可以绑定在所有的IP上
	//server_addr.sin_addr.s_addr=inet_addr("192.168.1.1");  //用于绑定到一个固定的IP inet_addr用于把数字加格式的ip转化为整形ip
	server_addr.sin_port = htons(TCP_port);         // 将本机上的short数据转化为网络上的short数据

	int reuse = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopet error\n");
		return -2;
	}
	/* 3.捆绑sockfd描述符到IP地址 */
	if (bind(socket_fd, (sockaddr_t *)(&server_addr), sizeof(sockaddr_t)) == -1)
	{
		fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
		return -3;
	}
	/* 4 设置允许连接的最大客户端数 */
	if (listen(socket_fd, TCP_MAX_Listen) == -1) {
		fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
		return -4;
	}
	/* 5.服务器阻塞 直到客户程序建立连接 */
	socklen_t sin_size = sizeof(sockaddr_in_t);
	printf("TCP Waiting for connect ... ...\n");
	//	等待建立连接
	if ((TCP_fd = accept(socket_fd, (sockaddr_t *)(&client_addr), &sin_size)) == -1)
	{
		fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
		return -5;
	}
	//fprintf(stderr, "Server get connection from %s\n", inet_ntoa(client_addr.sin_addr)); // 将网络地址转换为.字符串
	printf("TCP Server get connection from %s\n", inet_ntoa(client_addr.sin_addr));
	c_flag = true;		//TCP建立连接成功
	return 0;
}

//发送数据
int tcp_TCPClass::send(char src[],int coun)
{
	int ret;
	ret = write(TCP_fd, src, coun);//写函数
	//fprintf(print_off, "%s\n", src);
	if (ret < 0) 
	{
		fprintf(stderr, "Write TCP Error:%s\n", strerror(errno));
		return -1;
	}
	return 0;
}
