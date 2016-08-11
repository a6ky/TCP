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
#define CNT_LISTEN  16
#define MSG_MAXLEN  256

int main()
{
	int                listenfd, connfd;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t          addr_size; 
	char               msg[MSG_MAXLEN];
	int                msg_size;
	
	listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenfd == -1){
		perror("socket");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, SRV_ADDR, &(servaddr.sin_addr));
	servaddr.sin_port = htons(SRV_PORT);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind");
		exit(-1);
	}

	if (listen(listenfd, CNT_LISTEN) == -1) {
		perror("listen");
		exit(-1);
	}

	addr_size = sizeof(cliaddr);
	connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &addr_size);
	if (connfd == -1){
		perror("accept");
		exit(-1);
	}

	printf("Connected: %d\n", connfd);

	while (1) {
		msg_size = recv(connfd, msg, MSG_MAXLEN, 0);
		if (msg_size == 0)
			break;
		if (msg_size == -1) {
			perror("recv");
			exit(-1);
		} else
			printf("recv message from client: %s\n", msg);
		send(connfd, msg, msg_size, 0);
	}    
    
	close(listenfd);
	exit(0);
}
