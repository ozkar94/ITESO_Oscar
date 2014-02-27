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

#define BUFFERSIZE 1048576

int main(int args, char *argv[]) {

	u_int port; // Tipo de unisgned int que guarda el valor del puerto
	int server; // Guarda el descriptor del socket del servidor
	int client; //Guarda el descriptor del sockets del cliente
	int localerror;  //Variable para errores
	struct sockaddr_in server_addr;  /* Estructura que guarda:
									* sin_family = AF_INET: Tipo de conexion, conexiones ya sean en la misma maquina o en diversas maquinas
									* sin_port = numero del puerto a utilizar, se debe hacer la conversion con htons(port)
									* sin_addr.s_addr = La direccion del cliente. INADDR_ANY: acepta cualquier ip, pero es para el server
									*/
	socklen_t clienteLen;	
	int status; // Variable que guarda el status del descriptor, si se creo correctamente o no
	
	int readBytes = 0;  //Variable auxiliar para leer bytes
	int totalReadBytes = 0;	 //Variable auxiliar para leer bytes
	int totalWriteBytes = 0;  //Variable auxiliar para escribir bytes
	int writeBytes = 0;  //Variable auxiliar para escribir bytes
	char *readBuffer = NULL; //Variable auxiliar para leer el buffer
	int file;  //Variable para el archivo
	

	//Validamos los Arguemntos
	if(args < 3) {
		fprintf(stderr,"Error: Missing Arguments\n");
		fprintf(stderr,"\tUSE: %s [ADDR] [PORT]\n",argv[0]);
		return 1;
	}
	
	port = atoi(argv[2]);
	if(port < 1 || port > 65535) {
		fprintf(stderr, "Port %i out of range (1-65535)\n",port);
		return	1;
	}
	
	//Abrimos el socket
	server = socket(AF_INET, SOCK_STREAM, 0);
	if(server == -1)
	{
		printf("Error: Socket can't be opened\n");
		return 1;
	}
	
	//Abrimos el archivo
	if((file = open("rec2.jpg",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1) {
		localerror = errno;
		fprintf(stderr,"Can't open file for write (%s)\n",strerror(localerror));
		return 1;
	}
	
	//Nos preparamos para la conexion
	server_addr.sin_family = AF_INET;
	status = inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(port);
	
	status = connect(server, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(status != 0)
	{
		printf("Error: Can't connect\n");
		return 1;
	}
	
	//Empezamos a recibir	
		readBuffer = (char *) calloc(1,BUFFERSIZE);

		while((readBytes = read(server,readBuffer,BUFFERSIZE)) > 0) {
			totalWriteBytes = 0;
			printf("leyendo...\n");
			while(totalWriteBytes < readBytes) {
				writeBytes = write(file,readBuffer+totalWriteBytes,readBytes-totalWriteBytes);
				totalWriteBytes += writeBytes;
				printf("Escribiendo...\n");
			}
			totalReadBytes += readBytes;
			printf("Archivo Recibido\n");
		}
		
		//Cerramos
		free(readBuffer);
		close(file);
		close(client);		
}

