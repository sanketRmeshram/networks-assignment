run:
	cc -std=c99  -lpthread -c -o rsocket.o rsocket.c
	ar -rcs libsocket.a rsocket.o
	gcc -std=c99 user1.c -o user1 -L. -lsocket -lpthread
	gcc -std=c99 user2.c -o user2 -L. -lsocket -lpthread
