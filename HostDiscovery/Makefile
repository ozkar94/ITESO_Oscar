
#Variable para guardar los objetos a compilar
OBJS = HostDiscover.c UDPSocketsSuit.c 
OBJS2 = HostDiscoverClient.c UDPSocketsSuit.c

#Variable para el compulador
CC = gcc

#Variable para guardar las opciones del compilador
CCFLAGS = -Wall -o
#-Wall indica al compilador que muestre todos los warnings existentes

all: $(OBJS) HostDiscover Client

HostDiscover: $(OBJS)
		$(CC) $(OBJS) $(CCFLAGS) HostDiscover
Client: $(OBJS2)
		$(CC) $(OBJS2) $(CCFLAGS) Client