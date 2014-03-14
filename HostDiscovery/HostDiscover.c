/**
* UDP Broadcast Sender
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "UDPSocketsSuit.h"

int main(int args, char *argv[]){
	
	int udpSocket;
	struct sockaddr_in broadcastAddr;
	socklen_t broadlen = sizeof(broadcastAddr);
	
	//Variables para el envio de datos al broadcast
	char *broadcastIp;
	u_int broadcastPort;
	//char *sendString;
	
	//Variables para configuracion del broadcast
	int broadcastPermission;
	//u_int sendStringLen;
	
	int status;
	//int i;
	char recv [50];
	
	char clientIp [20];
	
	if(args < 2)
	{
		fprintf(stderr, "Error: Missing Arguments\n");
		fprintf(stderr, "\tUse: %s [PORT]\n", argv[0]);
		return 1;
	}
	
	//Guardamos los valores dados en sus respectivas variables
	broadcastIp = "255.255.255.255";
	broadcastPort = atoi(argv[1]);
	
	udpSocket = newUDPClientSocket4();
	
	//Aqui se dan los permisos para usar el broadcast
	broadcastPermission = 1;
	status = setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission));
	if(status == -1)
	{
		printf("Error: Can't set Broadcast Option\n");
		return 1;
	}
	
	//Prepararamos la estructura para el broadcast
	memset(&broadcastAddr, 0, sizeof(broadcastAddr));
	broadcastAddr.sin_family = AF_INET;
	inet_pton(AF_INET, broadcastIp, &broadcastAddr.sin_addr.s_addr);
	broadcastAddr.sin_port = htons(broadcastPort);
	
	//sendStringLen = strlen(sendString);
	//for(i = 0; i < 5; i++){
		status = sendto(udpSocket, "Hello!", 6, 0, (struct sockaddr *)&broadcastAddr, broadlen);
		//printf("Send %i bytes to broadcast address\n", status);
		
		sleep(2);
		status = recvfrom(udpSocket, recv, sizeof(recv), 0, (struct sockaddr *)&broadcastAddr, &broadlen);
		printf("%s ", recv);
		
		inet_ntop(AF_INET, &(broadcastAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
		printf("IP Address: %s\n", clientIp);
	//}
	
	closeUDPSocket4(udpSocket);
}