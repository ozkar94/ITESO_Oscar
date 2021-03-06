#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "NewSockets.h"

#define BUFFERSIZE 1048576

int main(int args, char *argv[]) {

	u_int port; // Tipo de unisgned int que guarda el valor del puerto
	int client; // Guarda el descriptor del socket del servidor

	int status; // Variable que guarda el status del descriptor, si se creo correctamente o no
	
	int readBytes = 0;  //Variable auxiliar para leer bytes
	int totalReadBytes = 0;	 //Variable auxiliar para leer bytes
	int totalWriteBytes = 0;  //Variable auxiliar para escribir bytes
	int writeBytes = 0;  //Variable auxiliar para escribir bytes
	char *readBuffer = NULL; //Variable auxiliar para leer el buffer
	int file;  //Variable para el archivo
	char answer [50];
	char fileSize [50];
	

	//Validamos los Arguemntos
	if(args < 4) {
		fprintf(stderr,"Error: Missing Arguments\n");
		fprintf(stderr,"\tUSE: %s [ADDR] [PORT] [FILE]\n",argv[0]);
		return 1;
	}
	
	port = atoi(argv[2]);
	if(port < 1 || port > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",port);
		return	1;
	}
	
	//Abrimos el socket
	client = newTCPClientSocket4(argv[1], port);
	
	//Preguntamos por el archivo
	status = write(client, argv[3], strlen(argv[3]));
	
	//Esperamos la respuesta
	status = read(client, answer, sizeof(answer));
	printf("Server says: %s\n", answer);
	
	if((strcmp(answer, "Not Found\r\n")) == 0)
	{
		printf("File not found in the server, closing connection\n");
		closeTCPSocket(client);
		return 1;
	}
	
	//Esperamos a que nos envien el tamaño del archivo
	status = read(client, answer, sizeof(answer));
	printf("File Size: %s\n", answer);
	strcpy(fileSize, answer);
	printf("%s\n", fileSize);
	
	//Aceptamos el envio
	status = write(client, "OK\r\n", 6);
	
	//Abrimos el archivo
	if((file = open(argv[3],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
		printf("Can't create file\n");
		return 1;
	}
	
	//Empezamos a recibir	
		readBuffer = (char *) calloc(1,BUFFERSIZE);

		while((readBytes = read(client,readBuffer,BUFFERSIZE)) > 0) {
			totalWriteBytes = 0;
			printf("leyendo...\n");
			while(totalWriteBytes < readBytes) {
				writeBytes = write(file,readBuffer+totalWriteBytes,readBytes-totalWriteBytes);
				totalWriteBytes += writeBytes;
				printf("Escribiendo...\n");
			}
			totalReadBytes += readBytes;
			if(totalReadBytes == atoi(fileSize))
				break;
		}
	
		printf("File received\n");
		
		//Nos despedimos del Server
		status = write(client, "Bye\r\n", 6);
		
		//Esperamos despedida del Server
		status = read(client, answer, sizeof(answer));
		printf("Server says: %s\n", answer);
		
		//Cerramos
		free(readBuffer);
		close(file);
		closeTCPSocket(client);		
}
