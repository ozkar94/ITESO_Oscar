
#Variable para guardar los objetos a compilar
OBJS = WebServerResumido.o NewSockets.o HTTPSocketsSuit.o

#Variable para el compulador
CC = gcc

#Variable para guardar las opciones del compilador
CCFLAGS = -Wall -o
#-Wall indica al compilador que muestre todos los warnings existentes

all: $(OBJS) WebServer

WebServer: $(OBJS)
		$(CC) $(OBJS) $(CCFLAGS) WebServer