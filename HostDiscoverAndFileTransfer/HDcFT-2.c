/*
 * HDcFT.c
 *
 *      Author: Eduardo Angeles Cabrera
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "lsDir-2.h"
#include "Sockets-2.h"

int main(int args, char *argv[]){

	int child;

	if(argumentos(args,argv,5,"[BROADCAST PORT] [TCP PORT] [NAME] [DIR ROUTE]")==-1)
		return 1;

	child = fork();

	if(child==-1){
		printf("No se pudo crear el hilo");

		return 1;
	}

	if(child==0){
		
		//Hilo que se encarga de recibir los mensajes en Broadcast por UDP

		char *nombre;
		char archivos[10];

		int udpSocket;

		struct sockaddr_in sender;
		socklen_t senderlen = sizeof(sender);
		u_int senderPort;

		char buffer [255];
		char senderIp [20];


		udpSocket = UDPnewSockServ(argv[1]);

		while(1){
			bzero(buffer, 255);
			recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr *)&sender, &senderlen);

			inet_ntop(AF_INET, &(sender.sin_addr), senderIp, INET_ADDRSTRLEN);
			senderPort = ntohs(sender.sin_port);

			printf("%s\n", buffer);

			fprintf(stderr, "Recibimos de [%s:%i] el mensaje: \"%s\"\n", senderIp, senderPort, buffer);

			//printf("Recibiendo\n");

			if((strstr("Hello from:", buffer))==NULL){

				sprintf(archivos, "%d", cntDir(argv[4]));
				nombre = calloc(strlen(archivos)+strlen(argv[4])+strlen(argv[3])+29,1);
				sprintf(nombre, "Hi %s\r\nPuerto: %s Archivos: %d\r\n\r\n", argv[3], argv[2], cntDir(argv[4]));
				sendto(udpSocket, nombre, strlen(nombre), 0, (struct sockaddr *)&sender, senderlen);
			}
		}

	}

	if(child>0){

		//Hilo que se encarga de la transferencia de archivos por TCP

		int i;
		int ext = 1;  //Variable de salida
		int socket;  //Socket para eñ servidor
		int client;  //Socket para la conexion con el cliente
		int file;
		int status; 
		char buffer[1024];
		char* wantItem;
		char fileSize [10];
		char cmd[15];  //Variable para guardar el comando a utilizar

		wantItem = (char *) calloc(1,255);

		struct stat fileStats; //Estructura para conocer propiedades de un archivo, usaremos st_size para el tamaño

		socket = TCPnewSockServ(argv[2]);
		//printf("Esperando conexion de un cliente\n");
		while(1){
			client = acceptClient(socket);
		//printf("Cliente conectado!\n");

			bzero(buffer, sizeof(buffer));
		//printf("Escuchando!!!\n");
			status = read(client, buffer, sizeof(buffer));

			printf("%s!! \n", buffer);

			if((strstr("PING", buffer))!=NULL){
			//printf("Enviando pong!\n");
				status = write(client, "PONG", strlen("PONG"));
			}

			do{		
				//Limpiando variables para usarlas
				ext = 1;
				bzero(buffer, sizeof(buffer));		
				bzero(cmd, sizeof(cmd));
				bzero(wantItem, sizeof(wantItem));
				bzero(fileSize, sizeof(fileSize));
				
				status = read(client, cmd, sizeof(cmd));
				printf("%s\n", cmd);
				bzero(buffer, sizeof(buffer));

				if((strcmp("FILELIST", cmd))==0){
					status = read(client, buffer, sizeof(buffer));  //Recibimos el GET
					printf("Enviando lista\n");
					status = write(client, "OK FILELIST\r\n", strlen("OK FILELIST\r\n"));  //Enviamos el OK
					TCPsendDir(client, argv[4]);  //Enciamos la lista en base a lsDir-2.h
				}

				if((strcmp("GETFILE", cmd))==0){
					status = read(client, buffer, sizeof(buffer));  //Recibimos el GET
					wantItem = strtok(buffer, " ");
					for(int i = 0; i < 2; i++)
						wantItem = strtok(NULL, " ");  //Separamos la cadena para encontrar el nombre del archivo
					printf("El cliente quiere: %s\n", wantItem);
					file = openArchServ(wantItem);  //Abrimos el archivo en base a Sockets-2.h
					if(file != -1)
						status = write(client, "OK GETFILE\r\n", strlen("OK GETFILE\r\n"));  //Enviamos el OK
					else{
						status = write(client, "Not Found\r\n", strlen("Not Found\r\n"));  //Enviamos not found como muestra de error al abrir el archivo
						break;
					}


					sleep(1);  //Sleep para evitar que los write y read se pierdan y vayan a la par
			//Indicamos a la estructura cual sera el archivo para que obtenga sus propiedades
					fstat(file, &fileStats);

					sprintf(fileSize, "%jd", (intmax_t)fileStats.st_size);
					status = write(client, fileSize, sizeof(fileSize));  //Enviamos el tamaño del archivo	
					printf("File Size: %s bytes\n", fileSize);	

					sleep(1);  //Sleep para evitar que los write y read se pierdan y vayan a la par
			//Aqui iria lo del MD5
					status = write(client, "MD5", strlen("MD5"));  //Enviamos el MD5
			//status = write(client, buffer, sizeof(buffer)); //Enviar MD5

			//iniciamos la transferencia del archivo
					printf("Enviando archivo\n");
					sleep(1);  //Sleep para evitar que los write y read se pierdan y vayan a la par
					sendFile(file, client, fileSize);  //Enviamos archivo en base a Sockets-2.h
					close(file);
				}

				if((strcmp("GETFILEPART", cmd))==0){

				}

				if((strcmp("GETFILESIZE", cmd))==0){
					status = read(client, buffer, sizeof(buffer));  //Recibimos el GET
					wantItem = strtok(buffer, " ");
					for(int i = 0; i < 2; i++)
						wantItem = strtok(NULL, " ");
					printf("El cliente quiere el tamaño del archivo: %s\n", wantItem);
					file = openArchServ(wantItem);
					if(file != -1)
						status = write(client, "OK GETFILESIZE\r\n", strlen("OK GETFILESIZE\r\n"));  //Enviamos el OK
					else
						status = write(client, "Not Found\r\n", strlen("Not Found\r\n"));  //Enviamos not found como muestra de error al abrir el archivo

			//Indicamos a la estructura cual sera el archivo para que obtenga sus propiedades
					fstat(file, &fileStats);

					sprintf(fileSize, "%jd", (intmax_t)fileStats.st_size);	
					printf("File Size: %s bytes\n", fileSize);	
					status = write(client, fileSize, sizeof(fileSize));  //Enviamos el tamaño del archivo
				}

				if((strstr("EXIT", cmd))!=NULL){
					close(client);  //Cerramos la conexion con el cliente
					ext = 0;
				}

			}while(ext != 0);
	}

			close(socket);

		}

	return 0;
}
