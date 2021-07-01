#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

void logexit(const char *msg);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);

char** strsplit( const char* s, const char* delim );
char** strsplit_count( const char* s, const char* delim, size_t* nb );
size_t ocurrencesOfSubstring(char* originalString, char* delim);