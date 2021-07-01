#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500
#define true 1
#define false !true

void usage(int argc, char **argv) {
	printf("usage: %s <v4|v6> <server port>\n", argv[0]);
	printf("example: %s v4 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

// used to store a new location
typedef struct dataWrapper {
	int deleted;
	int yCoordinate;
	int xCoordinate;
} DATA;

DATA **allLocations;

unsigned locationsSize = 0;

typedef struct node {
	DATA data;
	struct node *next;
} NODE;

NODE *head;

void init(NODE **head) { *head = NULL; }

char* print_list() {
	NODE *temp;
    char* output = (char*) malloc(BUFSZ);
	for (temp = head; temp; temp = temp->next) {
		//if (temp->data.deleted != true)
			sprintf(&output[strlen(output)], "%d %d", temp->data.xCoordinate,
					  temp->data.yCoordinate);
	}
	strcat(output, "\n");

    return output;
}


void add(DATA data) {
	NODE *newNode = (NODE *)malloc(sizeof(NODE));
	if (newNode == NULL) {
		exit(0); // no memory available
	}
	newNode->data = data; // Link the data part
	newNode->next = NULL;

	NODE *temp = head;

	if (temp == NULL) {
		temp = newNode;
		return;
	}
	// Traverse to the last node
	while (temp->next != NULL)
		temp = temp->next;

	temp->next = newNode; // Link address part
}

NODE *free_list(NODE *head) {
	NODE *tmpPtr = head;
	NODE *followPtr;
	while (tmpPtr != NULL) {
		followPtr = tmpPtr;
		tmpPtr = tmpPtr->next;
		free(followPtr);
	}
	return NULL;
}

/**
 * returns 2 if exceeded
 * returns 1 if already
 * returns 0 if sucessful
 */
int addLocation(int x, int y) {

	DATA novaEntrada;

	novaEntrada.xCoordinate = x;
	novaEntrada.yCoordinate = y;
	novaEntrada.deleted = false;
	if (locationsSize == 50)
		return 2;
	locationsSize++;

	for (NODE *temp = head; temp != NULL; temp = temp->next) {
		DATA atual = temp->data;

		if (atual.xCoordinate == x && atual.yCoordinate == y && !atual.deleted) {
			return 1;
		}
	}
	add(novaEntrada);
	return 0;
}

// returns if its found
int rmLocation(int x, int y) {
	int found = false;

	for (NODE *temp = head; temp != NULL; temp = temp->next) {
		DATA atual = temp->data;

		if (atual.xCoordinate == x && atual.yCoordinate == y && !atual.deleted) {
			found = true;
			temp->data.deleted = true;
			locationsSize--;
		}
	}

	return found;
}

char* closestPoint(int x, int y){
    if(head == NULL)
        return "none";


    NODE* temp = head;
    int currentClosestX = head->data.xCoordinate;
    int currentClosestY = head->data.yCoordinate;

//pitágoras, nao preciso fazer sqrt pq vou comparar com outra expressão
    long closestDistance=  ( (temp->data.xCoordinate-x)*(temp->data.xCoordinate-x))+ ((temp->data.yCoordinate-y)* (temp->data.yCoordinate-y));

    for (NODE *temp; temp != NULL; temp = temp->next){
        if(temp->data.deleted == false){
            long newValue = ( (temp->data.xCoordinate-x)*(temp->data.xCoordinate-x))+ ((temp->data.yCoordinate-y)* (temp->data.yCoordinate-y));
            if(closestDistance > newValue ){
                closestDistance= newValue;
                 currentClosestX = temp->data.xCoordinate;
    currentClosestY = temp->data.yCoordinate;
            }
        }
    }

    char* output = (char*) malloc(BUFSZ);
    sprintf(output, "%d %d\n", currentClosestX, currentClosestY);

    return output;
}

char *parseInput(char *buf) {
	char *outputString = (char *)malloc(sizeof(char) * BUFSZ);
	if (strstr(buf, "list") > 0) {
	  return print_list();
 
	}

	// casos com dois argumentos, requerem split da entrada
	else {

		char **splittedInputBySpaces = strsplit(buf, " ");

		int splittedInputArgument = ocurrencesOfSubstring(buf, " ");

		printf("Array splitado: %d\n", splittedInputArgument);
		for (int i = 0; i < splittedInputArgument; i++) {
			printf("String: %s\n", splittedInputBySpaces[i]);
		}

		// casos de adicao
		// add
		if (strstr(buf, "add") > 0) {
			puts("Caso de adicao");
			int xParseado = atoi(splittedInputBySpaces[1]);
			int yParseado = atoi(splittedInputBySpaces[2]);
			int output = addLocation(xParseado, yParseado);

			if (output == 0) {
				sprintf(outputString, "%d %d added\n", xParseado, yParseado);
			} else if (output == 1) {
				sprintf(outputString, "%d %d already exists\n", xParseado,
						  yParseado);
			} else if (output == 2) {
				strcat(outputString, "limit exceeded\n");
			}

		}

		// rm
		else if (strstr(buf, "rm") > 0) {
			int xParseado = atoi(splittedInputBySpaces[1]);
			int yParseado = atoi(splittedInputBySpaces[2]);
			int output = rmLocation(xParseado, yParseado);

			if (output == 0) {
				sprintf(outputString, "%d %d removed\n", xParseado, yParseado);
			} else if (output == 1) {
				sprintf(outputString, "%d %d does not exist\n", xParseado,
						  yParseado);
			}

		}

		// query
		else if (strstr(buf, "query") > 0) {
            int xParseado = atoi(splittedInputBySpaces[1]);
			int yParseado = atoi(splittedInputBySpaces[2]);
            outputString = closestPoint(xParseado, yParseado);
		}

		else {
			perror(buf);
		}
	}

	return outputString;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}

	int enable = 1;
	if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
		logexit("setsockopt");
	}

	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != bind(s, addr, sizeof(storage))) {
		logexit("bind");
	}

	if (0 != listen(s, 10)) {
		logexit("listen");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("bound to %s, waiting connections\n", addrstr);

	struct sockaddr_storage cstorage;
	struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
	socklen_t caddrlen = sizeof(cstorage);

	int csock = accept(s, caddr, &caddrlen);
	if (csock == -1) {
		logexit("accept");
	}

	char caddrstr[BUFSZ];
	memset(caddrstr, '\0', BUFSZ);
	addrtostr(caddr, caddrstr, BUFSZ);

	printf("[log] connection from %s\n", caddrstr);

	char buf[BUFSZ];

	init(&head);

	while (1) {
		s = socket(storage.ss_family, SOCK_STREAM, 0);
		memset(buf, 0, BUFSZ);
		size_t count;
		// unsigned total;
		puts("Lendo mais uma mensagem");

		count = recv(csock, buf, BUFSZ, 0);

		if ( strstr(buf, "ill") > 0) {
            close(csock);
            puts("Saindo do programa.");
			break;
		}
		buf[count + 1] = '\0';

        //TODO: Strings com múltiplas quebras de linha

		char *response = parseInput(buf);

		printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

		// sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
		sprintf(buf, response);

		count = send(csock, buf, strlen(buf), 0);
		if (count != strlen(buf)) {
			logexit("send");
		}
	}
	
	exit(EXIT_SUCCESS);
}