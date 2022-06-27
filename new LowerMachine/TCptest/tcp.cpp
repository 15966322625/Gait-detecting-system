#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_port	3333
#define TCP_MAX_Listen 5
typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

int socket_fd;	//�豸��ʶ��
int TCP_fd;

int tcp_TCPClass::init()
{
	/*	1.�������˽��н���sockfd������	*/
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{	// AF_INET:IPV4	SOCK_STREAM:TCP	
		fprintf(stderr, "Socket error:%s\n\a", strerror(errno));
		return -1;
	}
	/*	2.����������� sockaddr �ṹ	*/
	bzero(&server_addr, sizeof(sockaddr_in_t));		//��ʼ�� ��0
	server_addr.sin_family = AF_INET;		//Internet IPV4
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//(���������ϵ�long����ת��Ϊ�����ϵ�long����)�������������������κ�ip��������  //INADDR_ANY ��ʾ��������������IP��ַ����������������԰󶨵����е�IP��
														//server_addr.sin_addr.s_addr=inet_addr("192.168.1.1");  //���ڰ󶨵�һ���̶�IP,inet_addr���ڰ����ּӸ�ʽ��ipת��Ϊ����ip
	server_addr.sin_port = htons(TCP_port);         // (���������ϵ�short����ת��Ϊ�����ϵ�short����)�˿ں�

	int reuse = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopet error\n");
		return -2;
	}
	
	if (bind(socket_fd, (sockaddr_t *)(&server_addr), sizeof(sockaddr_t)) == -1)
	{
		fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
		return -3;
	}
	/* 4.�����������ӵ����ͻ����� */
	if (listen(socket_fd, TCP_MAX_Listen) == -1) {
		fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
		return -4;
	}



	socklen_t sin_size = sizeof(sockaddr_in_t);
	printf("TCP Waiting for connect ... ...\n");


	if ((TCP_fd = accept(socket_fd, (sockaddr_t *)(&client_addr), &sin_size)) == -1)
	{
		fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
		return -5;
	}


	printf("TCP Server get connection from %s\n", inet_ntoa(client_addr.sin_addr));
	c_flag = true;		
	return 0;
}

void main(){

}