
#Variable para guardar los objetos a compilar
OBJS = HDcFT-2.o
OBJS2 = HDcFTCl.o

#Variable para el compulador
CC = gcc

#Variable para guardar las opciones del compilador
CCFLAGS = -Wall -o
#-Wall indica al compilador que muestre todos los warnings existentes

all: $(OBJS) HDcFTSer HDcFTCl

HDcFTSer: $(OBJS)
		$(CC) $(OBJS) $(CCFLAGS) HDcFTSer
HDcFTCl: $(OBJS2)
		$(CC) $(OBJS2) $(CCFLAGS) HDcFTCl