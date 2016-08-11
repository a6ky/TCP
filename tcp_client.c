#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SRV_PORT    1667
#define SRV_ADDR    "127.0.0.1"
#define MSG_MAXLEN  256

int main()
{
	int                sockfd;
	struct sockaddr_in servaddr;
	char               msg[MSG_MAXLEN];
	int                msg_size;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1){
		perror("socket");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, SRV_ADDR, &(servaddr.sin_addr));
	servaddr.sin_port = htons(SRV_PORT);

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("connect");
		exit(-1);
	}

	while (1) {
		scanf("%s", msg);
		printf("send message to server: %s\n", msg);
		send(sockfd, msg, strlen(msg) + 1, 0);
		msg_size = recv(sockfd, msg, MSG_MAXLEN, 0);
		if (msg_size == -1) {
			perror("recv");
			exit(-1);
		} else
			printf("recv message from server: %s\n", msg);
	}
	
	close(sockfd);
	exit(0);
}
