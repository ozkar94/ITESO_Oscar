/**
* UDP File Transfer Server
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
	int server;  //Variable para el socket del servidor
	
	int status;  //Variable para comprobacion de errores
	int file;  //Variable para abrir el archivo
	char answer[100];  //Variable para guardar la respuesta del cliente
	char *buffer;  //Guarda el buffer
	
	int readBytes, totalReadBytes;  //Variables auxiliares para leer bytes
	int writeBytes, totalWriteBytes;  //Variables auxiliares para escribir
	
	struct stat fileStats; //Estructura para conocer propiedades de un archivo, usaremos st_size para el tamaño
	char fileSize [10]; //Variable para guardar el tamaño del archivo
	
	int i = 1;
	char part [10];
	
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
	
	//Crear el socket
	server = newUDPServerSocket4(port);
	
	//Recibimos un mensaje
	//Primero creamos la estructura donde se alojaran los datos del cliente
	struct sockaddr_in client;
	//En este caso no debemos llenarla porque se llenara sola al conectarse el cliente
	//Ahora establecemos su tamaño
	socklen_t longitudClient = sizeof(client);
	//Ahora creamos el buffer donde se guardara el mensaje a recibir
	char wantItem[255];
	
	//Nos ponemos a la espera de un mensaje
	status = recvfrom(server, wantItem, 255, 0, (struct sockaddr *)&client, &longitudClient); 
	
	printf("The requested item is: %s\n", wantItem);
	
	//Contestamos
	file = open(wantItem, O_RDONLY);
	if(file == -1){
		strcpy(answer, "The file doesn't exist");
		closeUDPSocket4(server);
		return 1;
	}
	else
		strcpy(answer, "OK, file found");

	//Enviamos la respuesta al archivo
	status = sendto(server, answer, 100, 0, (struct sockaddr *)&client, longitudClient);
	
	//Indicamos a la estructura cual sera el archivo para que obtenga sus propiedades
	fstat(file, &fileStats);
	
	//Enviamos el tamaño del archivo
	sprintf(fileSize, "%jd", (intmax_t)fileStats.st_size);	
	printf("File Size: %s\n", fileSize);	
	status = sendto(server, fileSize, sizeof(fileSize), 0, (struct sockaddr *)&client, longitudClient);
	
	//Iniciamos la transferecia
	buffer = (char *)calloc(1, BUFFERSIZE);
	
	totalReadBytes = 0;
	while((readBytes = read(file, buffer, BUFFERSIZE)) > 0) {
		totalWriteBytes = 0;
		sprintf(part, "%i", i);  //Convetimos el int a string
		printf("Sending package number: %s\n", part);  //Imprimimos el numero de paquete
		while(totalWriteBytes < readBytes) {
			writeBytes = sendto(server, buffer+totalWriteBytes, readBytes-totalWriteBytes, //enviamos la info
									0, (struct sockaddr *)&client, longitudClient);
			totalWriteBytes += writeBytes;
			status = sendto(server, part, 6, 0, (struct sockaddr *)&client, longitudClient);  //Enviamos el numero del paquete
		}
		status = recvfrom(server, answer, sizeof(answer), 0, (struct sockaddr *)&client, &longitudClient);  //Recibimos OK o ERROR del cliente
		if(strcmp(answer, "ERROR\r\n")==0)	{  //Si recibimos ERROR cancelamos la transferencia
			printf("Transmission Error\n");
			closeUDPSocket4(server);
			return 1;
		}
		totalReadBytes += readBytes;
		i++; //Aumentamos el contador de partes
	}
	
	printf("The file has been sent\n");
	closeUDPSocket4(server);
}