#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define BUFSZ 500

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	char addrstr[BUFSZ];
	char buf[BUFSZ];
	// unsigned totalMessages = 0;

	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	addrtostr(addr, addrstr, BUFSZ);

	// printf("connected to %s", addrstr);
	while (1) {
		printf("mensagem: ");
		memset(buf, 0, BUFSZ);
		fgets(buf, BUFSZ - 1, stdin);

		// if (strstr(buf, "kill") > 0) {
		// 	break;
		// }

		// ver os chars enviados
		for (int i = 0; i < strlen(buf); i++) {
			// printf("[%d]", buf[i]);
		}

		int newLen = strlen(buf);
		// send
		size_t count = send(s, buf, newLen, 0);

		if (count != newLen) {
			logexit("send");
			puts("Erro no send");
		}

		memset(buf, 0, BUFSZ);

		// loop de recebimento da mensagem, até o total for recebido

		int index = 0;

		/*
				while ((count = recv(s, &buf[index], BUFSZ - index, 0)) > 0) {
				  printf("Current count: %ld\n", count);
				  index += count;

				}*/
		count = recv(s, buf, BUFSZ, 0);

		// printf("received %u bytes\n", totalMessages);
		puts(buf);
	}

	close(s);

	exit(EXIT_SUCCESS);
}
