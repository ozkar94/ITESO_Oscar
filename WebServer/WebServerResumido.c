/**
* Web Server
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
#include "HTTPSocketsSuit.h"

void clientAccess(const int clientSocket);

int main(){

	u_int port; 
	int serverSocket; 
	int clientSocket;

	int multi;

	port = 80;

		//Creamos el socket en base a NewSockets.h
	serverSocket = newTCPServerSocket4(port, 5);

		//Nos ponemos en espera de una conexion
	while(1){
		clientSocket = waitConnection4(serverSocket);

		//Creamos los hilos
		multi = fork();
		if(multi == -1) {
			printf("Can't fork\n");
			return 1;
			close(clientSocket);
			continue;
		}
		else if(multi == 0)
				//Soy el hijo
			clientAccess(clientSocket);
		else if(multi > 0)
				//Soy el padre
			close(clientSocket);			
	}	

	return 1;	

}

void clientAccess(const int clientSocket){
	char *firstLine;
	char *wantItem;
	char fileName [] = "WindowsXP.iso";
	int file;
	int readBytes;
	char *buffer;
	
	//Se asignan los espacios de memorias
	buffer = calloc(255,1);
	firstLine = calloc(255,1);
	wantItem = calloc(255,1);
	
	//Leemos el GET y guardamos la primera liena en base a la funcion en HTTPSocketsSuit
	firstLine = getReading(clientSocket);
	//strcpy(firstLine, getReading(clientSocket));
	
	//Ya que guardamos la primera linea, identificamos la solicitud del cliente en base a 
	//la funcion getProcessing de HTTPSocketsSuit
	wantItem = getProcessing(firstLine);
	
	//Pequeño debug para saber que se solicito
	printf("Archivo solicitado: %s\n", wantItem);
	
	if((strcmp(fileName, wantItem)==0))
	{
		//Enviamos las cabeceras indicadas confirmando que el archivo existe
		//Funcion sendHeader en HTTPSocketsSuit
		sendHeader("HTTP/1.1 200 OK\r\n", clientSocket);
		sendHeader("Content-Type: video/iso.segment\r\n", clientSocket);
		sendHeader("Content-Length: 715000000\r\n", clientSocket);
		sendHeader("\r\n", clientSocket);
		
		//Enviamos el archivo pedido
		file = open(fileName, O_RDONLY);
		if(file == -1)
			printf("Error: Can't open file\n");	

		while((readBytes = read(file, buffer, 255)) > 0){
			write(clientSocket, buffer, readBytes);
		}
		
	} else if((strcmp("HTTP", wantItem)==0))
	{
		sendHeader("HTTP/1.1 200 OK\r\n", clientSocket);
		sendHeader("Content-Type: text/html\r\n", clientSocket);
		sendHeader("Content-Length: 224\r\n", clientSocket);
		sendHeader("\r\n", clientSocket);
		
		//Enviamos el archivo pedido
		file = open("Pagina.html", O_RDONLY);
		if(file == -1)
			printf("Error: Can't open file\n");	

		while((readBytes = read(file, buffer, 255)) > 0){
			write(clientSocket, buffer, readBytes);
		}
	} else
		//Enviamos el header de error por medio de la funcion sendErrorHeader de HTTPSocketsSuit
		sendErrorHeader(clientSocket);

		
	//Se liberan los espacios de memoria	
	free(buffer);
	free(firstLine);
	close(clientSocket);
	
	return;
}
