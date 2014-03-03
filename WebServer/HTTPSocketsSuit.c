/**
* Archivo c que contiene funciones para facilitar
* el envio y lectura de headers de los web browsers
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "NewSockets.h"

char* getReading( const int clientSocket ){
	char *buffer;
	char *firstLine;
	int firstFlag;
	
	buffer = calloc(255,1);
	firstLine = calloc(255,1);
	firstFlag = 1;
	
	//Creamos un ciclo para leer todo el GET del cliente
	while(readTCPLine4(clientSocket, buffer, 254) > 0) {
		if(strcmp(buffer, "\r\n")==0)
			break;
		if(firstFlag == 1) {
			strcpy(firstLine, buffer);
			firstFlag = 0;
		}
		bzero(buffer, 255);
	}
	
	return firstLine;
}

char* getProcessing( char* firstLine ){
	char* wantItem;
	
	wantItem = calloc(255,1);
	
	wantItem = strtok(firstLine, "/ ");
	for(int i = 0; i < 1; i++)
		wantItem = strtok(NULL, "/ ");
		
	return wantItem;
}

void sendHeader (char* header, const int clientSocket){
	char html [250];
	
	strcpy(html, header);
	write(clientSocket, html, strlen(html));
}

void sendErrorHeader(const int clientSocket){
	char error [] = "HTTP/1.1 404 NOT FOUND\r\n";
	char endOfHeader [10];
	int errorFile;
	char errorFileName [] = "Error.html";
	char *buffer;
	int readBytes;
	
	buffer = calloc(255,1);
	
	write(clientSocket, error, strlen(error));
	
	strcpy(endOfHeader, "\r\n");
	write(clientSocket, endOfHeader, strlen(endOfHeader));
	
	//Enviar el archivo de error
	errorFile = open(errorFileName, O_RDONLY);
	if(errorFile == -1)
		printf("Error: Can't open file\n");
		
	while((readBytes = read(errorFile, buffer, 255)) > 0){
		write(clientSocket, buffer, readBytes);
	}
	
	free(buffer);
	
}

