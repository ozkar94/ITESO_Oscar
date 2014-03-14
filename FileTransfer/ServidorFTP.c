
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

#define BUFFERSIZE 1048576  //Tamaño fijo del buffer

int main(int args, char *argv[]) {
	
	u_int port; //Tipo de variable unsigned int,guarda el valor del puerto
	int server; //Guarda el descriptor del socket del servidor
	int client; //Guarda el descriptor del socket cliente

	int status; //Guarda descriptores para comprobar errores
	char archivo [200]; //Nombre del archivo a transferir
	int file;  //Variable para el archivo
	char *buffer;  //Variable para el buffer
	char answer [50];
	
	int readBytes, totalReadBytes;  //Variables auxiliares para leer bytes
	int writeBytes, totalWriteBytes;  //Variables auxiliares para escribir
	
	struct stat fileStats; //Estructura para conocer propiedades de un archivo, usaremos st_size para el tamaño
	char fileSize [10]; //Variable para guardar el tamaño del archivo
	
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
	server = newTCPServerSocket4(port, 5);

	
	//Nos ponemos en espera de una conexion
	while(1) {  //Ciclo para que nunca se cierre el servidor
		client = waitConnection4(server);
		
		//Esperamos para conocer el archivo a enviar
		status = read(client, archivo, sizeof(archivo));
		printf("The client wants: %s\n", archivo);		
		
		//Intentamos abrir el archivo para ver si existe o no
		file = open(archivo, O_RDONLY);
		if(file == -1)
		{
			printf("Error: Can't found the file %s\n", archivo);
			status = write(client, "Not Found\r\n", 15);
			closeTCPSocket(client);
			return 1;
		}
		
		//Si logramos abrir el archivo enviamos el OK
		status = write(client, "OK\r\n", 5);
		
		//Indicamos a la estructura cual sera el archivo para que obtenga sus propiedades
		fstat(file, &fileStats);
		
		//Enviamos el tamaño del archivo
		sprintf(fileSize, "%jd", (intmax_t)fileStats.st_size);	
		printf("File Size: %s\n", fileSize);	
		status = write(client, fileSize, sizeof(fileSize));
		
		//Recibimos el OK del cliente
		status = read(client, answer, sizeof(answer));
		printf("Client says: %s\n", answer);
		
		//Comenzamos el envio
		buffer = (char *) calloc(1, BUFFERSIZE);
		totalReadBytes = 0;
		while((readBytes = read(file, buffer, BUFFERSIZE))>0) {
			totalWriteBytes = 0;
			while(totalWriteBytes < readBytes) {
				writeBytes = write(client, buffer+totalWriteBytes, readBytes-totalWriteBytes);
				totalWriteBytes += writeBytes;
			}
			
			totalReadBytes += readBytes;	
		}
		
		printf("File sent\n");
		
		//Recibimos la despedida
		status = read(client, answer, sizeof(answer));
		printf("Client says: %s\n", answer);
		
		//Enviamos la despedida
		status = write(client, "Bye\r\n", 6);
		
		//Cerramos
		close(file);
		free (buffer);
		close(client);
				
	}
		
}