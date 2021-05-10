all: client.o server.o
server.o: server.c
	gcc -o server.o server.c
client.o: client.c
	gcc -o client.o client.c

clean:
	rm -rf *.o
	rm -rf *.soc
