/**
* Archivo c que facilita la creacion de sockets en UDP
* Sirve para Servidores y Clientes
*/

#include <stdio.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int newUDPServerSocket4(const u_short port){
	int socketUDPDescriptor;
	int status;
	struct sockaddr_in addr;
	
	socketUDPDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketUDPDescriptor == -1){
		printf("Error: The socket can't be opened\n");
		return 1;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	status = bind(socketUDPDescriptor, (struct sockaddr*)&addr, sizeof(addr));
	if(status != 0){
		printf("Error: Can't bind port\n");
		close(socketUDPDescriptor);
		return 1;
	}
	
	printf("Server UDP Socket Created\n");
	
	return socketUDPDescriptor;
}

void closeUDPSocket4(const int socketUDPDescriptor){
	close(socketUDPDescriptor);
	printf("Socket closed\n");
}

int newUDPClientSocket4(){
	int socketUDPDescriptor;
	int status;
	struct sockaddr_in addr;
	
	socketUDPDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketUDPDescriptor == -1){
		printf("Error: The socket can't be opened\n");
		return 1;
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = INADDR_ANY;
	
	status = bind(socketUDPDescriptor, (struct sockaddr*)&addr, sizeof(addr));
	if(status != 0){
		printf("Error: Can't bind port\n");
		close(socketUDPDescriptor);
		return 1;
	}
	
	printf("Client UDP Socket Created\n");
	
	return socketUDPDescriptor;
	
}