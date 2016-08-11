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
	int                listenfd, connfd[100];
	struct sockaddr_in servaddr;
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

    int i = 0, j;
    
    int maxfd = listenfd;
    fd_set rfd;

    while (1) {
        struct timeval tv;
        int status = 0;

        FD_ZERO(&rfd);
        FD_SET(listenfd, &rfd); 
        maxfd = listenfd;

        for(j = 0; j < i; j++) {
            if (connfd[j] > maxfd)
                maxfd = connfd[j];
            FD_SET(connfd[j], &rfd);
        }

        /* Ждем не больше 10 секунд. */ 
        tv.tv_sec = 10;     /* секунды */
        tv.tv_usec = 0;    /* микросекунды */
        status = select(maxfd + 1, &rfd, NULL, NULL, &tv);

        if (FD_ISSET(listenfd, &rfd)) {
            connfd[i] = accept(listenfd, NULL, NULL);
            if (connfd[i] == -1){
                 perror("accept");
                exit(-1);
            }
            printf("Connected: %d\n", connfd[i]);
            i++;
        }

        for(j = 0; j < i; j++)
            if (FD_ISSET(connfd[j], &rfd)) {    /* сокет готов для чтения */
                msg_size = recv(connfd[j], msg, MSG_MAXLEN, 0);
                if (msg_size == 0)
                    break;
                if (msg_size == -1) {
                    perror("recv");
                    close(connfd[j]);
                    exit(-1);
                } else {
                    printf("recv message from client: %s\n", msg);
                    send(connfd[j], msg, msg_size, 0);
                }
            }
    }
    
	close(listenfd);
	exit(0);
}
