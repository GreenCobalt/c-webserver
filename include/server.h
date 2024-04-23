#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/in.h>

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

#endif /* SERVER_H_ */