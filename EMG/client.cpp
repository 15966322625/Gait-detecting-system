#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define MYPORT  3333
#define BUFFER_SIZE 10
struct ANS
{
	char label;
	float time_process;
};

int main(int argc, char **argv)
{
	///¶¨Òåsockfd
	//char SERVER_IP[20] = argv[1];
	int sock_cli = socket(AF_INET, SOCK_STREAM, 0);
	int count = sizeof(ANS);
	///¶¨Òåsockaddr_in
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(MYPORT);  ///·þÎñÆ÷¶Ë¿Ú
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);  ///·þÎñÆ÷ip

	//printf("connect %d\n", argv[1], MYPORT);
	///Á¬½Ó·þÎñÆ÷£¬³É¹¦·µ»Ø0£¬´íÎó·µ»Ø-1
	if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect");
		exit(1);
	}
	printf("connect success!\n");
	char recvbuf[count];
	while (1)
	{
		int len = recv(sock_cli, recvbuf, count, 0);
		if(len != 0 && len != -1)
		{
			ANS *ans_label = (ANS *)recvbuf;
			printf("%c,====%f\n", ans_label->label,ans_label->time_process);
		}
	}
	close(sock_cli);
	return 0;
}
