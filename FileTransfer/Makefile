
#Variable para guardar los objetos a compilar
OBJS = ServidorFTP.c NewSockets.c
OBJS2 = ClienteFTP.c NewSockets.c

#Variable para el compulador
CC = gcc

#Variable para guardar las opciones del compilador
CCFLAGS = -Wall -o
#-Wall indica al compilador que muestre todos los warnings existentes

all: $(OBJS) ServidorFTP ClienteFTP

ServidorFTP: $(OBJS)
		$(CC) $(OBJS) $(CCFLAGS) ServidorFTP
ClienteFTP: $(OBJS2)
		$(CC) $(OBJS2) $(CCFLAGS) ClienteFTP