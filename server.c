
#include "include/server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// define server_constructor method that returns server_struct. sets variables and sets up server.address. then starts listening on port and address.
struct Server server_constructor(int domain, int type, int protocol, u_long interface, int port, int backlog, void (*run)(struct Server *server))
{
    
    struct Server server;

    server.domain = domain;
    server.type = type;
    server.protocol = protocol;

    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = server.domain;
    server.address.sin_port = htons(server.port);
    server.address.sin_addr.s_addr = htonl(server.interface);

    server.run = run;

    server.socket = socket(server.domain, server.type, server.protocol);
    if (server.socket < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }

    setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    if (bind(server.socket, (struct sockaddr *) &server.address, sizeof(server.address)) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    if (listen(server.socket, server.backlog) < 0) {
        perror("Failed to start listening");
        exit(1);
    }

    return server;
}