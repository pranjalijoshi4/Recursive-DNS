all: dhtserver3.o dhtserver2.o dhtserver1.o client1.o client2.o
	gcc -o dhtserver1.out -g dhtserver1.o -lsocket -lnsl -lresolv
	gcc -o dhtserver2.out -g dhtserver2.o -lsocket -lnsl -lresolv
	gcc -o dhtserver3.out -g dhtserver3.o -lsocket -lnsl -lresolv
	gcc -o client1.out -g client1.o -lsocket -lnsl -lresolv
	gcc -o client2.out -g client2.o -lsocket -lnsl -lresolv

dhtserver1.o: dhtserver1.c
	gcc -g -c -Wall dhtserver1.c

dhtserver2.o: dhtserver2.c
	gcc -g -c -Wall dhtserver2.c

dhtserver3.o: dhtserver3.c
	gcc -g -c -Wall dhtserver3.c

client1.o: client1.c
	gcc -g -c -Wall client1.c

client2.o: client2.c
	gcc -g -c -Wall client2.c

clean:
	rm -f *.o dhtserver1 dhtserver2 dhtserver3 client1 client2
