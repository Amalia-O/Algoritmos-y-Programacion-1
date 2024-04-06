CC = gcc
CFLAGS =  -std=c99 -pedantic -g -c -lSDL2 

all: hangon

hangon: main.o imagen.o paleta.o fondo.o ruta.o moto.o
	$(CC)  main.o paleta.o imagen.o fondo.o ruta.o moto.o -lSDL2 -o hangon -lm -ldl

main.o: main.c imagen.h paleta.h fondo.h config.h ruta.h
	$(CC)  $(CFLAGS) main.c 

imagen.o: imagen.c imagen.h 
	$(CC) $(CFLAGS) imagen.c 

paleta.o: paleta.c paleta.h
	$(CC) $(CFLAGS) paleta.c

fondo.o: fondo.c fondo.h
	$(CC) $(CFLAGS) fondo.c

ruta.o: ruta.c ruta.h
	$(CC) $(CFLAGS) ruta.c
	
moto.o: moto.c moto.h
	$(CC) $(CFLAGS) moto.c


clean:
	rm *.o
	rm hangon


