#include <stdio.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int newTCPServerSocket4(const u_short port, const int queue_size){
	int socketTCPDescriptor;
	int status;
	struct sockaddr_in addr;
	
	socketTCPDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(socketTCPDescriptor == -1){
		printf("Error: The socket can't be opened\n");
		return 1;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	status = bind(socketTCPDescriptor, (struct sockaddr*)&addr, sizeof(addr));
	if(status != 0){
		printf("Error: Can't bind port\n");
		return 1;
	}
	
	status = listen(socketTCPDescriptor, queue_size);
	if(status == -1)
	{
		printf("Error: Can't listen on socket\n");
		close(socketTCPDescriptor);
		return 1;
	}
	
	printf("Socket created\n");
	
	return socketTCPDescriptor;
}

void closeTCPSocket(const int socketTCPDescriptor){
	close(socketTCPDescriptor);
	printf("Socket closed\n");
}

int waitConnection4(int socket){
	int client;
	struct sockaddr addrClient;
	socklen_t addrLen;
	
	addrLen = sizeof(addrClient);
	
	bzero(&addrClient, sizeof(addrClient));
	client = accept(socket, &addrClient, &addrLen);
	if(client == -1)
	{
		printf("Error: Can't accept connection\n");
	}
	
	return client;
}

int newTCPClientSocket4(const char *ip, const u_short port){
	int serverSocket;
	int status;
	struct sockaddr_in serverAddr;
	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket == -1)
	{
		printf("Error: Socket can't be opened\n");
		return 1;
	}
	
	serverAddr.sin_family = AF_INET;
	status = inet_pton(AF_INET, ip, &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(port);
	
	status = connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(status != 0)
	{
		printf("Error: Can't connect\n");
		return 1;
	}
	
	printf("Connected to %s:%i\n",ip,port);
	
	return serverSocket;
	
}