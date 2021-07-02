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

void initLinkedList() { head = NULL; }

char *print_list() {
	NODE *temp;
	char *output = (char *)malloc(BUFSZ);
	memset(output, 0, BUFSZ);

	for (temp = head; temp != NULL; temp = temp->next) {
		sprintf(output + strlen(output), "%d %d ", temp->data.xCoordinate,
				  temp->data.yCoordinate);
	}
	strcat(output, "\n\0");

	return output;
}

void addToLinkedList(DATA data) {
	NODE *newNode = (NODE *)malloc(sizeof(NODE));
	if (newNode == NULL) {
		exit(0); // no memory available
	}
	newNode->data = data; // Link the data part
	newNode->next = NULL;

	NODE *temp = head;

	// caso da primeira inserção
	if (temp == NULL) {
		head = newNode;
		return;
	}
	// Traverse to the last node
	while (temp->next != NULL)
		temp = temp->next;

	temp->next = newNode; // Link address part
}

NODE *free_list() {
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
int addLocation(int x, int y, char *output) {

	DATA novaEntrada;

	novaEntrada.xCoordinate = x;
	novaEntrada.yCoordinate = y;

	// check to see if max size achieved
	if (locationsSize == 50) {
		strcat(output, "limit exceeded\n");

		return 2;
	}

	// check for existing values
	for (NODE *temp = head; temp != NULL; temp = temp->next) {
		DATA atual = temp->data;

		if (atual.xCoordinate == x && atual.yCoordinate == y) {
			sprintf(output, "%d %d already exists\n", x, y);

			return 1;
		}
	}
	locationsSize++;
	addToLinkedList(novaEntrada);
	sprintf(output, "%d %d added\n", x, y);

	return 0;
}

// returns if its found
int rmLocation(int x, int y, char *output) {
	int found = false;
	NODE *predecessor = head;

	if (head == NULL) {
		sprintf(output, "%d %d does not exist\n", x, y);
		return found;
	}

	// case where the head must be deleted
	if (head->data.xCoordinate == x && head->data.yCoordinate == y) {
		NODE *tempHead = head;
		head = head->next;
		free(tempHead);
		sprintf(output, "%d %d removed\n", x, y);
		return true;
	}

	// head->next all the way to the end
	for (NODE *current = head->next; current != NULL; current = current->next) {
		DATA atual = current->data;

		if (atual.xCoordinate == x && atual.yCoordinate == y) {
			if (current == head) {
				head = head->next;
				free(head);
			} else {
				predecessor->next = current->next;
				free(current);
			}

			found = true;
			locationsSize--;
			sprintf(output, "%d %d removed\n", x, y);

			return found;
		}
		// andar com o antecessor
		predecessor = predecessor->next;
	}
	// if it wasnt find so far, it does not exist.
	sprintf(output, "%d %d does not exist\n", x, y);

	return found;
}

int closestPoint(int x, int y, char *output) {
	if (head == NULL) {

		strcat(output, "none\n\0");
		return 1;
	}

	NODE *temp = head;
	int currentClosestX = head->data.xCoordinate;
	int currentClosestY = head->data.yCoordinate;

	// pitágoras, nao preciso fazer sqrt pq vou comparar com outra expressão
	long closestDistance =
		 ((temp->data.xCoordinate - x) * (temp->data.xCoordinate - x)) +
		 ((temp->data.yCoordinate - y) * (temp->data.yCoordinate - y));

	for (temp; temp != NULL; temp = temp->next) {
		int currentX = temp->data.xCoordinate;
		int currentY = temp->data.yCoordinate;

		long newValue =
			 (currentX - x) * (currentX - x) + (currentY - y) * (currentY - y);

		if (closestDistance > newValue) {
			closestDistance = newValue;
			currentClosestX = temp->data.xCoordinate;
			currentClosestY = temp->data.yCoordinate;
		}
	}

	sprintf(output, "%d %d\n", currentClosestX, currentClosestY);

	return 0;
}

char *parseInput(char *buf) {
	char *outputString = (char *)malloc(sizeof(char) * BUFSZ);
	if (strstr(buf, "list") != NULL) {
		outputString = print_list();
		return outputString;

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
		// addToLinkedList
		if (strstr(buf, "add") > 0) {
			puts("Caso de adicao");
			int xParseado = atoi(splittedInputBySpaces[1]);
			int yParseado = atoi(splittedInputBySpaces[2]);
			int output = addLocation(xParseado, yParseado, outputString);

		}

		// rm
		else if (strstr(buf, "rm") > 0) {
			int xParseado = atoi(splittedInputBySpaces[1]);
			int yParseado = atoi(splittedInputBySpaces[2]);
			int output = rmLocation(xParseado, yParseado, outputString);

		}

		// query
		else if (strstr(buf, "query") > 0) {
			int xParseado = atoi(splittedInputBySpaces[1]);
			int yParseado = atoi(splittedInputBySpaces[2]);
			closestPoint(xParseado, yParseado, outputString);
		}

		else {
			strcat(outputString, "Operação não suportada");
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

	initLinkedList();

	while (1) {
		// s = socket(storage.ss_family, SOCK_STREAM, 0);
		memset(buf, 0, BUFSZ);
		size_t count;
		// unsigned total;
		puts("Lendo mais uma mensagem");

		int finishedMessage = false;
		int totalReceived = 0;

		while (!finishedMessage) {
			count = recv(csock, buf, BUFSZ, 0);
			totalReceived += count;
			for (int i = 0; i < count; i++)
				printf("[%d] ", buf[i]);

			puts("\n");

			if (count == 0) {
				puts("Count chegou 0");
				finishedMessage = true;

			} else if (strstr(buf, "\n") > 0) {
				puts("A mensagem contém um \\n");
				finishedMessage = true;
			} else if (buf[count - 1] == '\n') {
				puts("O último char da mensagem é um barra n");
				finishedMessage = true;
			}
		}
		buf[totalReceived + 1] = '\0';
		if (strstr(buf, "ill") > 0) {
			close(csock);
			// puts("Saindo do programa.");
			break;
		}

		// TODO: Strings com múltiplas quebras de linha
		int numberOfReturnTerminatedStrings =
			 ocurrencesOfSubstring(buf, "\n") - 1;
		printf("Há %d substrings terminadas com barra n\n",
				 numberOfReturnTerminatedStrings);
		char **arrayOfInputStrings = strsplit(buf, "\n");
		int currentSubinput = 0;

		// execute till all subtrings are pars ed
		while (numberOfReturnTerminatedStrings > currentSubinput) {
			// resetting the buffer
			memset(buf, 0, BUFSZ);
			// moving the current substring to buffer
			strcat(buf, arrayOfInputStrings[currentSubinput]);
			char *response = parseInput(buf);

			// printf("[msg] %s, %d bytes: %s\n", caddrstr, (int) count, buf);
			// limpar buf antes de receber a resposta

			// sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
			memset(buf, 0, BUFSZ);
			strcat(buf, response);

			count = send(csock, buf, strlen(buf), 0);
			if (count != strlen(buf) || count < 1) {
				logexit("send");
			}
			currentSubinput++;
		}
	}

	exit(EXIT_SUCCESS);
}
