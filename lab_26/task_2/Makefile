all: client.o server.o
server.o: server.c includes.h
	gcc -o server.o server.c
client.o: client.c includes.h
	gcc -o client.o client.c

clean:
	rm -rf *.o
