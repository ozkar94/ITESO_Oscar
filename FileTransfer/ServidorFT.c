
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

//#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdlib.h>
//#include <stdio.h>

#define BUFFERSIZE 1048576  //Tamaño fijo del buffer

int main(int args, char *argv[]) {
	
	u_int port; //Tipo de variable unsigned int,guarda el valor del puerto
	int server; //Guarda el descriptor del socket del servidor
	int client; //Guarda el descriptor del socket cliente
	struct sockaddr_in myAddr; /*Estructura que guarda los datos de mi direccion
								*sin_family = AF_INET ; cualquier tipo de conexion
								*sin_port = htons(port); convertir el formato del puerto
								*sin_addr.s_addr = htonl(INADDR_ANY); convertir cualquier direccion al formato adecuado
								*/
	struct sockaddr clientAddr; // Estructura como la anterior para el cliente
	socklen_t clientLen;  //Variable que guarda la longitud del cliente
	int status; //Guarda descriptores para comprobar errores
	char Archivo [] = "imgpr.jpg"; //Nombre del archivo a transferir
	int file;  //Variable para el archivo
	char *buffer;  //Variable para el buffer
	
	int readBytes, totalReadBytes;  //Variables auxiliares para leer bytes
	int writeBytes, totalWriteBytes;  //Variables auxiliares para escribir
	
	//Abrimos el archivo
	file = open(Archivo, O_RDONLY);
	if(file == -1)
	{
		printf("Error: Can't open the file\n");
		return 1;
	}
	
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
	
	//Abrir el socket
	server = socket(AF_INET, SOCK_STREAM, 0);
	if(server == -1)
	{
		printf("Error: The socket can't be opened\n");
		return 1;
	}
	
	//Avisamos al sistema que utilizaremos el socket
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(port);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	status = bind(server, (struct sockaddr*)&myAddr, sizeof(myAddr));
	if(status != 0)
	{
		printf("Error: Can't bind port\n");
		return 1;
	}
	
	//Nos ponemos en modo de escucha
	status = listen(server, 5);
	if(status == -1)
	{
		printf("Error: Can't listen on socket\n");
		close(server);
		return 1;
	}
	
	//Nos ponemos en espera de una conexion
	while(1) {  //Ciclo para que nunca se cierre el servidor
		client = accept(server, &clientAddr, &clientLen);
			if(client == -1)
		{
			printf("Error: Can't acept connection\n");
			close(server);
			return 1;
		}
		
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
			printf("Archivo Enviado\n");	
		}
		
		//Cerramos
		close(file);
		free (buffer);
		close(client);
				
	}
		
}
