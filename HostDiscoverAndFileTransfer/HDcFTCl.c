#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Sockets-2.h"

int main(int args, char *argv[]){

		if(argumentos(args,argv,3,"[BROADCAST PORT] [NAME]")==-1)  //Validacion de argumentos en base a Sockets-2.h
			return 1;
		
		char *nombre;
		char *broadcastIp;
		int broadcastPermission;
		int status;

		u_int puertoUDP;
		int udpSocket;

		struct sockaddr_in broadcastAddr;
		socklen_t broadlen = sizeof(broadcastAddr);

		char buffer [255];
		char clientIp [20];
				
		broadcastIp = "255.255.255.255";

		puertoUDP = verifPuerto(argv[1]);  //Verificar el puerto en base a Sockets-2.h
		if(puertoUDP == -1)
			return	1;

		udpSocket = UDPnewSockCli();  //Creamos el socket UDP

				//Aqui se dan los permisos para usar el broadcast
				broadcastPermission = 1;
				status = setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission));
				if(status == -1)
				{
					printf("Error: Can't set Broadcast Option\n");
					return 1;
				}

				//Prepararamos la estructura para el broadcast
				memset(&broadcastAddr, 0, sizeof(broadcastAddr));
				broadcastAddr.sin_family = AF_INET;
				inet_pton(AF_INET, broadcastIp, &broadcastAddr.sin_addr.s_addr);
				broadcastAddr.sin_port = htons(puertoUDP);
				
					//Enviamos
					sprintf(buffer, "Hello from: %s", argv[2]);
					status = sendto(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&broadcastAddr, broadlen);
					status = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&broadcastAddr, &broadlen);
					printf("%s ", buffer);

					inet_ntop(AF_INET, &(broadcastAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
					printf("IP Address: %s\n", clientIp);
			
////////////////////////////////////Aqui inicia la parte de TCP///////////////////////////////////////////////			
				
		char *puertoTCP;
		puertoTCP = calloc(10,1);
			
		printf("Servidor a conectar..\nDireccion IP: ");
		scanf("%s", clientIp);  //Pedimos la IP a la cual conectarnos
		printf("Puerto: ");
		scanf("%s", puertoTCP);  //Pedimos el puerto al cual conectarnos

		int socketTCP;
		int file;
		int ext = 1;
		char ping[6] = "PING";
		char cmd[15];
		char MD5[4];
		char answer[100];
		char lista[2000];
		char wantItem[40];
		char fileSize [50];
		
		printf("Conectando... Puerto %s, Dir IP %s\n", puertoTCP, clientIp);

		socketTCP = TCPnewSockCli(puertoTCP, clientIp);
		printf("Socket creado\n");
		bzero(buffer, sizeof(buffer));
		//printf("Enviando ping!\n");
		status = write(socketTCP, ping, sizeof(ping));
		//printf("Recibiendo pong!\n %d", status);
		status = read(socketTCP, buffer, sizeof(buffer));
		printf("%s!!\n", buffer);
		
		if((strcmp("PONG", buffer)==0))
			printf("Conexion establecida y funcionando\n");
		else
			return -1;
		do{
			//Limpiamos la variables antes de utilizarlas
			bzero(answer, sizeof(answer));
			bzero(cmd, sizeof(cmd)); 
			bzero(wantItem, sizeof(wantItem));
			bzero(fileSize, sizeof(fileSize));
			bzero(lista, sizeof(lista));
			
			//Enlistamos los comandos a utilizar
			printf("Comando a utilizar..\n1.-FILELIST\n2.-GETFILE\n3.-GETFILEPART\n4.-GETFILESIZE\n5.-EXIT\nNombre del comando: ");
			scanf("%s", cmd);
			status = write(socketTCP, cmd, sizeof(cmd));  //Enviamos el comando a ejecutar

			if((strstr("FILELIST", cmd))!=NULL){
				status = write(socketTCP, "FILELIST\r\n", strlen("FILELIST\r\n"));
				printf("Recibiendo lista\n");
				status = read(socketTCP, answer, sizeof(answer)); //recibimos el OK o Not Found
				printf("Server says: %s\n", answer);
				status = read(socketTCP, lista, sizeof(lista)); //Recibimos la lista
				printf("%s\n", lista);
			}

			else if((strstr("GETFILE", cmd))!=NULL){
				printf("Nombre del archivo a solicitar: ");
				scanf("%s", wantItem);
				sprintf(answer, "GETFILE Nombre: %s \r\n",wantItem);
				status = write(socketTCP, answer, sizeof(answer)); //Enviamos el GET
				bzero(answer, sizeof(answer));
				status = read(socketTCP, answer, sizeof(answer)); //recibimos el OK o Not Found
				printf("Server says: %s\n", answer);

				if((strcmp(answer, "Not Found\r\n")) == 0){
					printf("File not found in the server, closing connection\n");
					break;
				}

				status = read(socketTCP, fileSize, sizeof(fileSize));  //Recibimos el tamaño
				printf("File Size: %s bytes\n", fileSize);
				status = read(socketTCP, MD5, sizeof(MD5));  //Recibimos el MD5
				printf("MD5: %s\n", MD5);
								
				//Aqui empezamos a recibir
				file = openArchCli(wantItem);  //Abrimos el archivo en base a Sockets-2.h
				printf("Recibiendo archivo...\n");
				sleep(2);
				recvFile(file, socketTCP, fileSize);  //Enviamos en base a Sockets-2.h
				close(file);  //Cerramos el archivo para finalizar la trasnferencia
				
			}

			else if((strstr("GETFILEPART", cmd))!=NULL){

			}

			else if((strstr("GETFILESIZE", cmd))!=NULL){
				printf("Nombre del archivo a solicitar tamaño: ");
				scanf("%s", wantItem);
				sprintf(answer, "GETFILESIZE Nombre: %s \r\n",wantItem);
				status = write(socketTCP, answer, sizeof(answer)); //Enviamos el GET
				bzero(answer, sizeof(answer)); //Limpiamos para recibir
				status = read(socketTCP, answer, sizeof(answer)); //recibimos el OK o Not found
				printf("Server says: %s\n", answer);

				if((strcmp(answer, "Not Found\r\n")) == 0){
					printf("File not found in the server, closing connection\n");
					break;
				}
					
				status = read(socketTCP, answer, sizeof(answer));  //Recibimos el tamaño
				printf("Tamaño del archivo: %s bytes\n", answer);
			}

			else if((strstr("EXIT", cmd))!=NULL){
				close(socketTCP);
				return 0;
			}

			else 
				printf("Comando incorrecto...\n");  //Validacion
		
		}while(ext!=0);
		
		close(socketTCP);

	return 0;
}
