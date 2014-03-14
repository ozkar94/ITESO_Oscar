/**
* UDP File Transfer Client
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "UDPSocketsSuit.h"

#define BUFFERSIZE 1024

int main(int args, char *argv[]){
	
	u_int port;  //Variable para el puerto
	int client;  //Variable para el socket del cliente
	
	int status;  //Variable para comprobacion de errores
	char *answer;  //Variable para guardar las respuestas del server
	answer = calloc(100,1);
	
	struct sockaddr_in server; //Estructura del servidor
	int readBytes, totalReadBytes = 0;  //Variables auxiliares para leer bytes
	int writeBytes, totalWriteBytes;  //Variables auxiliares para escribir
	char *readBuffer = NULL; 
	int file;
	char fileSize [50];
	
	char parte [10];
	int contadorParte = 1;
	
	//Validamos los argumentos
	if(args < 4) {
		fprintf(stderr, "Error: Missing Arguments\n");
		fprintf(stderr, "\tUse: %s [IP ADDRESS] [PORT] [FILE]\n", argv[0]);
		return	1;
	}
	
	port = atoi(argv[2]);
	if(port < 1 || port > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",port);
		return	1;
	}
	
	
	//Creamos el socket
	client = newUDPClientSocket4();
	
	//Construimos la estructura del servidor
	server.sin_family = AF_INET;
	status = inet_pton(AF_INET, argv[1], &server.sin_addr.s_addr);
	server.sin_port = htons(port);
	
	socklen_t longitudServer = sizeof(server);

	//Preguntamos por el archivo	
	status = sendto(client, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&server, sizeof(server));
	
	//Ahora esperamos respuesta
	status = recvfrom(client, answer, 100, 0, (struct sockaddr *)&server, &longitudServer);
	printf("Server says: %s\n", answer);
	
	//Esperamos el tamaño del archivo
	status = recvfrom(client, fileSize, sizeof(fileSize), 0, (struct sockaddr *)&server, &longitudServer);
	printf("File Size: %s\n", fileSize);
	
	//Abrimos el archivo donde se guardaran los datos
	if((file = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
		printf("Can't create file!\n");
		return 1;
	}
	
	//Empezamos a recibir
	readBuffer = (char *) calloc(1, BUFFERSIZE);
	
	while((readBytes = recvfrom(client, readBuffer, BUFFERSIZE, 0, (struct sockaddr *)&server, &longitudServer)) > 0) {
		totalWriteBytes = 0;
		printf("reading...\n");
		status = recvfrom(client, parte, 10, 0, (struct sockaddr *)&server, &longitudServer);  //Recibimos el numero de paquete que se enviara
		printf("Receiving package number: %i of %i\n", atoi(parte), contadorParte);	//Imprimimos el paquete enviado	y el que deberia tocar
		while(totalWriteBytes < readBytes) {
			writeBytes = write(file, readBuffer+totalWriteBytes, readBytes-totalWriteBytes);  //Escribimos en el documento
			totalWriteBytes += readBytes;
			printf("Writing...\n");
		}
		if(atoi(parte) == contadorParte){	 //Comprobamos que no se haya brincado una parte
			status = sendto(client, "OK\r\n", 5, 0, (struct sockaddr *)&server, sizeof(server));  //Enviamos mensaje de OK porque todo va bien
		}
		else
			status = sendto(client, "ERROR\r\n", 5, 0, (struct sockaddr *)&server, sizeof(server));  //Enviamos error en la transmision
		totalReadBytes += readBytes;
		contadorParte++;  //Aumentamos el contador del numero de parte
		if(totalReadBytes == atoi(fileSize))  //Condicion para salir del while
			break;
	}
	printf("File Received\n");
	closeUDPSocket4(client);
	
}