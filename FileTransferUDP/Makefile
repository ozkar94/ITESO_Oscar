
#Variable para guardar los objetos a compilar
OBJS = UDPServer.c UDPSocketsSuit.c
OBJS2 = UDPClient.c UDPSocketsSuit.c

#Variable para el compulador
CC = gcc

#Variable para guardar las opciones del compilador
CCFLAGS = -Wall -o
#-Wall indica al compilador que muestre todos los warnings existentes

all: $(OBJS) UDPServer UDPClient

UDPServer: $(OBJS)
		$(CC) $(OBJS) $(CCFLAGS) UDPServer
UDPClient: $(OBJS2)
		$(CC) $(OBJS2) $(CCFLAGS) UDPClient