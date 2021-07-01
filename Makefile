all:
	gcc -Wall -c common.c
	gcc -Wall cliente.c common.o -o cliente
	gcc -Wall servidor.c common.o -o servidor
	gcc -Wall server-mt.c common.o -lpthread -o server-mt

clean:
	rm common.o cliente servidor server-mt
