#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#define	SUCCESS 1
#define FAILURE 0
#define	BUFFER_SZ 512

class server {
	private:
		struct sockaddr_in servAddr, cliAddr;
		int portNo, cliLen, sockfd, tempSockfd, status, p[2], t;
		pthread_t thread_id;
		static void *routeRequest(void *sock_fd);

	public:
		server(int port);
		int bringup(); 
};

server::server(int port) {
	portNo = port;
	memset((char *)&servAddr, 0, sizeof (servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(portNo);
	cout<<"Server Created"<<endl;
}

int server::bringup() {
	int t1;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Unable to create socket");
		return FAILURE;
	}
	if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof (servAddr)) < 0) {
		perror("Binding failed");
		return FAILURE;
	}	
	listen(sockfd, 5);
	cliLen = sizeof (cliAddr);
	t = 2;
	if (pipe(p)) {
		printf("Pipe failed\n");
		exit(0);
	}
	while (true) {
		tempSockfd = accept(sockfd, (struct sockaddr *)&cliAddr, (socklen_t *)&cliLen);
		if( pthread_create( &thread_id , NULL ,  server::routeRequest ,(void *) &tempSockfd) < 0)
        	{
           		 perror("could not create thread");
            		return FAILURE;
        	}
	}	
	return SUCCESS;
}

void *server::routeRequest(void *sock_fd) {
	int flag;
	int sock = *(int*)sock_fd;
	char buff[BUFFER_SZ];
	
	memset(buff, 0, sizeof (buff));
	while (read(sock, buff, sizeof (buff)) > 0) {
		printf("Here is the message\n");
		printf("%s\n", buff);
		memset(buff, 0, sizeof (buff));
		strcpy(buff, "Thank you!\n");
		flag = write(sock, buff, sizeof (buff));
		if (flag < 0) {
			perror("Write failed");
			return 0;
		}
		memset(buff, 0, sizeof (buff));
	}
	if (sock == 0) {	
		printf("Client disconnected\n");
		fflush(stdout);
	}
	return 0;
} 
