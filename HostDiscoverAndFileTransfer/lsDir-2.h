/*
 * lsDir.h
 *
 *      Author: Eduardo Angeles Cabrera
 */

#ifndef LSDIR_H_
#define LSDIR_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

void procesoArchivo(char *ruta, struct dirent *archivo);
long fileSize(char *fname);
void sendProcesoArchivo(char *ruta, struct dirent *archivo, int socket);

char archivos[1024];
char *aux=NULL;
char *lista = NULL;
int flag=1;

int TCPsendDir(int socket, char *ruta){
	/* Con un puntero a DIR abriremos el directorio */
	DIR *dir;
	/* en *ent habrá información sobre el archivo que se está "sacando" a cada momento */
	struct dirent *ent;

	int bytes;

	/* Empezaremos a leer en el directorio actual */
	dir = opendir (ruta);

	if (dir == NULL)
	printf("No puedo abrir el directorio\n");

	/*Leemos todos los archivos uno a la vez*/
	while ((ent = readdir (dir)) != NULL){
		/* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
		if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
		/* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
		sendProcesoArchivo(ruta, ent, socket);
		}
	}
	//printf("Lo que se mandaría:\n%s",lista);
	bytes = write(socket,lista, strlen(lista));


	closedir (dir);

	return bytes;
}

void sendProcesoArchivo(char *ruta, struct dirent *archivo, int socket)
{
  long ftam=-1;
  char *nombrecompleto;
  int tmp=strlen(ruta);



   nombrecompleto=malloc(tmp+strlen(archivo->d_name)+2); /* Sumamos 2, por el \0 y la barra de directorios (/) no sabemos si falta */
   if (ruta[tmp-1]=='/')
     sprintf(nombrecompleto,"%s%s", ruta, archivo->d_name);
   else
     sprintf(nombrecompleto,"%s/%s", ruta, archivo->d_name);

  ftam=fileSize(nombrecompleto);


  if (ftam>=0){
	  sprintf(archivos,"%30s (%ld bytes)\n", archivo->d_name, ftam);

	  if(flag==0){
		  aux = calloc(strlen(lista),1);
		  strcpy(aux,lista);
		  lista = NULL;
		  lista = calloc(strlen(archivos)+strlen(aux),1);
		  sprintf(lista,"%s%s",aux,archivos);
	  }
	  else if(flag==1){
		  lista = calloc(strlen(archivos),1);
		  sprintf(lista,"%s",archivos);
		  flag=0;
	  }

  }
  else{
	  sprintf(archivos,"%30s (No info.)\n", archivo->d_name);

	  if(flag==0){
	  		  aux = calloc(strlen(lista),1);
	  		  strcpy(aux,lista);
	  		  lista = NULL;
	  		  lista = calloc(strlen(archivos)+strlen(aux),1);
	  		  sprintf(lista,"%s%s",aux,archivos);
	  	  }
	  	  else if(flag==1){
	  		  lista = calloc(strlen(archivos),1);
	  		  sprintf(lista,"%s",archivos);
	  		  flag=0;
	  	  }
  }

}

int cntDir(char *ruta) 	//char *ruta = "./hola/";
{
	/* Con un puntero a DIR abriremos el directorio */
	DIR *dir;
	/* en *ent habrá información sobre el archivo que se está "sacando" a cada momento */
	struct dirent *ent;

	int cantidad=0;


	/* Empezaremos a leer en el directorio actual */
	dir = opendir (ruta);

	/* Miramos que no haya error */
	if (dir == NULL)
	printf("No puedo abrir el directorio\n");

	/* Una vez nos aseguramos de que no hay error, ¡vamos a jugar! */
	/* Leyendo uno a uno todos los archivos que hay */
	while ((ent = readdir (dir)) != NULL){
		cantidad++;
	}
	closedir (dir);

	return cantidad;
}

void lsDir(char *ruta) 	//char *ruta = "./hola/";
{
	/* Con un puntero a DIR abriremos el directorio */
	DIR *dir;
	/* en *ent habrá información sobre el archivo que se está "sacando" a cada momento */
	struct dirent *ent;


	/* Empezaremos a leer en el directorio actual */
	dir = opendir (ruta);

	/* Miramos que no haya error */
	if (dir == NULL)
	printf("No puedo abrir el directorio\n");

	/* Una vez nos aseguramos de que no hay error, comenzamos*/
	/* leyendo uno a uno todos los archivos que hay */
	while ((ent = readdir (dir)) != NULL){
		/* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
		if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) ){
		/* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */
		procesoArchivo(ruta, ent);
		}
	}
	closedir (dir);
}

void procesoArchivo(char *ruta, struct dirent *archivo)
{
  /* Para "procesar", o al menos, hacer algo con el archivo, vamos a decir su tamaño en bytes */
  /* para ello haremos lo que vemos aquí: http://totaki.com/poesiabinaria/2010/04/tamano-de-un-fichero-en-c/ */
  long ftam=-1;
  char *nombrecompleto;
  int tmp=strlen(ruta);


   nombrecompleto=malloc(tmp+strlen(archivo->d_name)+2); /* Sumamos 2, por el \0 y la barra de directorios (/) no sabemos si falta */
   if (ruta[tmp-1]=='/')
     sprintf(nombrecompleto,"%s%s", ruta, archivo->d_name);
   else
     sprintf(nombrecompleto,"%s/%s", ruta, archivo->d_name);

  ftam=fileSize(nombrecompleto);
  if (ftam>=0)
	  printf("%30s (%ld bytes)\n", archivo->d_name, ftam);
  else
	  printf("%30s (No info.)\n", archivo->d_name);

}

long fileSize(char *fname)
{
  FILE *fich;
  long ftam=-1;

  fich=fopen(fname, "r");
  if (fich)
    {
      fseek(fich, 0L, SEEK_END);
      ftam=ftell(fich);
      fclose(fich);
    }
  else
    printf("ERRNO: %d - %s\n", errno, strerror(errno));
  return ftam;
}


#endif /* LSDIR_H_ */
