/*
 * Sockets.h
 *
 *      Author: Eduardo Angeles Cabrera
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

int readTCPLine4(const int socket, char *buffer, const unsigned int maxRead ) {
	char *ptr;
	int byte;
	int readBytes;

	ptr = buffer;
	readBytes = 0;

	while((byte = read(socket,ptr,1)) > 0) {
		if(*ptr == '\n' || *ptr == '\0') {
			break;
		}

		if(readBytes == maxRead) {
			break;
		}

		ptr++;
		readBytes++;
	}

	return readBytes;
}

int TCPnewSockServ(char *puerto){

	int tcpSocket;
	struct sockaddr_in Server_addr;
	int localerror;
	int status;
	int prto;

	prto = atoi(puerto);
	if(prto < 1 || prto > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",prto);
		return	-1;
	}


	tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

	if(tcpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return -1;
	}

	bzero(&Server_addr, sizeof(Server_addr));
	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	Server_addr.sin_port = htons(prto);

	status = bind(tcpSocket, (struct sockaddr *)&Server_addr, sizeof(Server_addr));

	if(status != 0) {
		localerror = errno;
		fprintf(stderr,"Can't Bind Port: %s\n",strerror(localerror));
		close(tcpSocket);
		return -1;
	}

	status = listen(tcpSocket,5);
	if(status == -1) {
		localerror = errno;
		fprintf(stderr,"Can't listen on socket(%s)\n",strerror(localerror));
		close(tcpSocket);
		return -1;
	}

	return tcpSocket;
}

int TCPnewSockCli(char *puerto, char* ip){

	int tcpSocket;
	int localerror;
	struct sockaddr_in Client_addr;
	int prto;

	prto = atoi(puerto);
	if(prto < 1 || prto > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",prto);
		return	-1;
	}

	tcpSocket = socket(PF_INET, SOCK_STREAM, 0);

	if(tcpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return -1;
	}

	bzero(&Client_addr, sizeof(Client_addr));
	Client_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(Client_addr.sin_addr.s_addr));
	Client_addr.sin_port = htons(prto);

	connect(tcpSocket, (struct sockaddr *)&Client_addr, sizeof(Client_addr));

	return tcpSocket;

}

int UDPnewSockServ(char *puerto){

	int udpSocket;
	struct sockaddr_in Server_addr;
	int localerror;
	int status;
	int prto;

	prto = atoi(puerto);
	if(prto < 1 || prto > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",prto);
		return	-1;
	}


	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(udpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return -1;
	}

	Server_addr.sin_family = AF_INET;
	Server_addr.sin_port = htons(prto);
	Server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	status = bind(udpSocket, (struct sockaddr*)&Server_addr, sizeof(Server_addr));

	if(status != 0) {
		localerror = errno;
		fprintf(stderr,"Can't Bind Port: %s\n",strerror(localerror));
		close(udpSocket);
		return -1;
	}

	return udpSocket;
}

int UDPnewSockCli(){

	int udpSocket;
	struct sockaddr_in Client_addr;
	int localerror;
	int status;

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(udpSocket == -1) {
		localerror = errno;
		fprintf(stderr, "Error: %s\n",strerror(localerror));
		return -1;
	}

	Client_addr.sin_family = AF_INET;
	Client_addr.sin_port = 0;
	Client_addr.sin_addr.s_addr = INADDR_ANY;

	status = bind(udpSocket, (struct sockaddr*)&Client_addr, sizeof(Client_addr));

	if(status != 0) {
		localerror = errno;
		fprintf(stderr,"Can't Bind Port: %s\n",strerror(localerror));
		close(udpSocket);
		return -1;
	}

	return udpSocket;
}

int acceptClient(tcpSocket){

	int Cliente;
	struct sockaddr_in Client_addr;
	socklen_t clienteLen;

	bzero(&Client_addr, sizeof(Client_addr));
	Cliente = accept(tcpSocket, (struct sockaddr *)&Client_addr, &clienteLen);

	return Cliente;
}

int openArchCli(char *nombre){

	int file;
	int localerror;

	if((file = open(nombre, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) ==-1){

		localerror = errno;
		fprintf(stderr, "No se pudo abrir el archivo para recivir (%s)\n", strerror(localerror));
		return -1;
	}

	return file;

}

int openArchServ(char *nombre){

	int file;
	int localerror;

	if((file = open(nombre, O_RDONLY)) ==-1){

		localerror = errno;
		fprintf(stderr, "No se pudo abrir el archivo para mandar (%s)\n", strerror(localerror));
		return -1;
	}

	return file;
}

void sendFile(int file, int cliente, char *tamano){

	char *readBuffer = NULL;
	int totalReadBytes = 0;
	int totalWrotenBytes = 0;
	int readBytes = 0;
	int wrotenBytes = 0;
	
	readBuffer = (char *) calloc(1, 10240);

	while((readBytes = read(file,readBuffer,10240)) > 0){
		//printf("RB: %d\n", readBytes);
		totalWrotenBytes = 0;
		while(totalWrotenBytes < readBytes){
			wrotenBytes = write(cliente,readBuffer+totalWrotenBytes, readBytes-totalWrotenBytes);
			//printf("WB: %d\n", wrotenBytes);
			totalWrotenBytes += wrotenBytes;
		}
		totalReadBytes += readBytes;
	}

	fprintf(stderr,"Se mandaron %d Bytes\n",totalReadBytes);

}

void recvFile(int file, int cliente, char *tamano){

	char *readBuffer = NULL;
	int totalReadBytes = 0;
	int totalWrotenBytes = 0;
	int readBytes = 0;
	int wrotenBytes = 0;
	
	readBuffer = (char *) calloc(1,10240);

	while((readBytes = read(cliente,readBuffer,10240)) > 0) {
		totalWrotenBytes = 0;
		//printf("RB: %d\n", readBytes);
		while(totalWrotenBytes < readBytes) {
			wrotenBytes = write(file,readBuffer+totalWrotenBytes,readBytes-totalWrotenBytes);
			//printf("WB: %d\n", wrotenBytes);
			totalWrotenBytes += wrotenBytes;
		}
		totalReadBytes += readBytes;
		//printf("TRB: %d\n", totalReadBytes);
		//printf("%d\n", atoi(tamano));
		//printf("tamano-TRB: %d\n", (atoi(tamano)-totalReadBytes));

		if((atoi(tamano)-totalReadBytes) < 10240){
			readBytes = read(cliente, readBuffer, (atoi(tamano)-totalReadBytes));
			//printf("RB: %d\n", readBytes);
			wrotenBytes = write(file,readBuffer+totalWrotenBytes,readBytes);
			//printf("WB: %d\n", wrotenBytes);
			totalReadBytes += readBytes;
			//printf("Dentro del If TRB: %d\n", totalReadBytes);
		}

		if(totalReadBytes == atoi(tamano))
			break;
	}

	fprintf(stderr,"Se recibieron %d Bytes\n",totalReadBytes);
		
}

int argumentos(int args, char *argv[], int total, char *texto){
	if(args < total)
	{
		fprintf(stderr, "Error: Missing Arguments\n");
		fprintf(stderr, "\tUse: %s %s\n", argv[0], texto);
		return -1;
	}
	else if(args > total)
	{
		fprintf(stderr, "Error: Too many Arguments\n");
		fprintf(stderr, "\tUse: %s %s\n", argv[0], texto);
		return -1;
	}
	return 0;
}

int verifPuerto(char *puerto){
	int prto;

	prto = atoi(puerto);
	if(prto < 1 || prto > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",prto);
		return	-1;
	}

	return prto;
}

#endif /* SOCKETS_H_ */
