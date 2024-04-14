#ifndef _SERVER_H
#define _SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>

typedef unsigned long u_long;

// define server struct with necessary data types
typedef struct Server
{
    int domain;
    int type;
    int protocol;
    int backlog;

    u_long interface;
    int port;
    struct sockaddr_in address;
    int socket;

    void (*run)(struct Server *server);
} Server;

Server server_constructor(int domain, int type, int protocol, u_long interface, int port, int backlog, void (*run)(Server *server));

#endif /* _SERVER_H */