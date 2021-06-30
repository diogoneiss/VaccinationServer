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

#define BUFSZ 1024

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
	unsigned totalMessages = 0;

	while (1) {
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

		printf("mensagem: ");
		memset(buf, 0, BUFSZ);
		fgets(buf, BUFSZ - 1, stdin);

		if (strstr(buf, "kill") > 0) {
			break;
		}

		size_t count = send(s, buf, strlen(buf) + 1, 0);
		if (count != strlen(buf) + 1) {
			logexit("send");
		}

		memset(buf, 0, BUFSZ);
		unsigned total = 0;

		// loop de recebimento da mensagem, até o total for recebido
		while (1) {
			count = recv(s, buf + total, BUFSZ - total, 0);
			if (count == 0) {
				// Connection terminated.
				break;
			}
			total += count;
			totalMessages += count;
		}
		close(s);
	}
	

	printf("received %u bytes\n", totalMessages);
	puts(buf);

	exit(EXIT_SUCCESS);
}
