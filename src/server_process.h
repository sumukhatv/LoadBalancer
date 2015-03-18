#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define	SUCCESS 1
#define FAILURE 0
#define	BUFFER_SZ 512

class server {
	private:
		struct sockaddr_in servAddr, cliAddr;
		int portNo, cliLen, sockfd, tempSockfd, status, p[2], t;
		pid_t pid, res;
		int routeRequest(int sock_fd); 
	
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
		printf("Parent; t = %d\n", t);
		fopen(p[1], "w");
		write(p[1], &t, sizeof (t));
		close(p[1]);
		if (tempSockfd < 0) {
			perror("Accept failed");
			return FAILURE;
		}
		pid = fork();
		if (pid < 0) {
			perror("Fork failed");
			return FAILURE;
		}
		if (pid == 0) {
			close(sockfd);
			open(p[0], "r");
			read(p[0], &t1, sizeof (t1));
			close(p[0]);
			t1--;
			printf("Child; t = %d\n", t1);
			write(p[0], &t1, sizeof (t1));
			if (routeRequest(tempSockfd) == FAILURE)
				printf("Handler Failed!");
			exit(0);
		}
		else {
			close(tempSockfd);
		}
		open(p[0], "r");
		read(p[0], &t, sizeof (t));
		close(p[0]);
	}	
	return SUCCESS;
}

int server::routeRequest(int sock_fd) {
	int flag;
	char buff[BUFFER_SZ];
	
	memset(buff, 0, sizeof (buff));
	while (read(sock_fd, buff, sizeof (buff)) > 0) {
		printf("Here is the message\n");
		printf("%s\n", buff);
		memset(buff, 0, sizeof (buff));
		strcpy(buff, "Thank you!\n");
		flag = write(sock_fd, buff, sizeof (buff));
		if (flag < 0) {
			perror("Write failed");
			return FAILURE;
		}
		memset(buff, 0, sizeof (buff));
	}
	if (sock_fd == 0) {	
		printf("Client disconnected\n");
		fflush(stdout);
	}
	return SUCCESS;
}
