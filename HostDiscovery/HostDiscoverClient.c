/* tenemos que dar permisos al socket para poder mandar broadcast
* se hace con la funcion: setsockopt
* setsockopt(bcSock, SOL_SOCKET, SO)
*/

/**
* UDP Broadcast Listener
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
	
int main(int args, char *argv[]) {
	
	u_int port;
	int udpSocket;
	
	//Estructuras del cliente y el servidor, en este caso, listener y sender
	struct sockaddr_in sender;
	socklen_t senderlen = sizeof(sender);
	u_int senderPort;
	
	char buffer [255];
	char senderIp [20];
	
	int status;
	
	//Validamos los argumentos
	if(args < 2) {
		fprintf(stderr, "Error: Missing Arguments\n");
		fprintf(stderr, "\tUse: %s [PORT]\n", argv[0]);
		return	1;
	}
	
	port = atoi(argv[1]);
	if(port < 1 || port > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",port);
		return	1;
	}
	
	//Creamos el socket
	udpSocket = newUDPServerSocket4(port);
	
	while(1){
		bzero(buffer, 255);
		status = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr *)&sender, &senderlen);
		
		//Tenemos que guardar la ip y el puerto del sender
		inet_ntop(AF_INET, &(sender.sin_addr), senderIp, INET_ADDRSTRLEN);
		senderPort = ntohs(sender.sin_port);
		
		//Mostramos quien nos envio el mensaje
		printf("Recibimos de [%s:%i] el mensaje %s\n", senderIp, senderPort, buffer);
		
		status = sendto(udpSocket, "MacBook Pro de Ozkar", 40, 0, (struct sockaddr *)&sender, senderlen);
		
	}
	
	closeUDPSocket4(udpSocket);
	
}