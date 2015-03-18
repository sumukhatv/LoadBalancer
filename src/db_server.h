#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define	SUCCESS 1
#define FAILURE 0
#define	BUFFER_SZ 512

class db_server {
	private:
		struct sockaddr_in servAddr, cliAddr;
		int portNo, cliLen, sockfd, tempSockfd;
		pid_t pid;
		int do_this(int sock_fd); 
	
	public:
		db_server(int port);
		int bringup(); 
};

db_server::db_server(int port) {
	portNo = port;
	memset((char *)&servAddr, 0, sizeof (servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(portNo);
	cout<<"DB Server Created"<<endl;
}

int db_server::bringup() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Unable to create socket");
		return FAILURE;
	}
	if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof (servAddr)) < 0) {
		perror("Binding failed");
		return FAILURE;
	}	
	listen(sockfd, 1);
	cliLen = sizeof (cliAddr);
	here:
	while (true) {
		printf("Tokens = %d\n", token);
		tempSockfd = accept(sockfd, (struct sockaddr *)&cliAddr, (socklen_t *)&cliLen);
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
			printf("Token inside child = %d\n", token);
			if (serveData(tempSockfd) == FAILURE)
				perror("Handler Failed!");
			return SUCCESS;
		}
		else {
			close(tempSockfd);
		}
	}
	return SUCCESS;
}

int db_server::serveData(int sock_fd) {
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
