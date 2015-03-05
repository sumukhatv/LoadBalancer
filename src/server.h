#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define	SUCCESS 1
#define FAILURE 0
#define	BUFFER_SZ 512

class server {
	private:
		struct sockaddr_in servAddr, cliAddr;
		int portNo, cliLen, sockfd, tempSockfd;
		pid_t pid;
		int do_this(int sock_fd); 
	
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
	while (true) {
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
			if (do_this(tempSockfd) == FAILURE)
				printf("Handler Failed!");
			return SUCCESS;
		}
		else {
			close(tempSockfd);
		}
	}	
	return SUCCESS;
}

int server::do_this(int sock_fd) {
	int flag;
	char buff[BUFFER_SZ];
	
	memset(buff, 0, sizeof (buff));
	flag = read(sock_fd, buff, sizeof (buff));
	if (flag < 0) {
		perror("Read failed");
		return FAILURE;
	}
	printf("Here is the message\n");
	printf("%s\n", buff);
	strcpy(buff, "Thank you!");
	flag = write(sock_fd, buff, sizeof (buff));
	if (flag < 0) {
		perror("Write failed");
		return FAILURE;
	}	
	return SUCCESS;
}
