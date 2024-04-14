
#include "include/server.h"
#include "include/strfunc.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

void run(Server *server)
{
    while (1)
    {
        // set variables and stuff
        char req[32768];
        struct sockaddr *sock_addr = (struct sockaddr *)&server->address;
        socklen_t address_length = (socklen_t)sizeof(server->address);

        // acept and read from socket
        int conn_socket = accept(server->socket, sock_addr, &address_length);
        printf("Connection received\n");
        read(conn_socket, req, 32768);

        // parse http request
        char *requested = strreplace(req, "\n\n", "|");
        char *REQLINE = strtok(requested, "\n");
        char *HEADERS = strtok(NULL, "|");
        char *REQBODY = strtok(NULL, "|");

        // get requesting socket ip
        struct sockaddr_in *pV4Addr = (struct sockaddr_in *)sock_addr;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);

        // generate response html
        char html[32768];
        snprintf(html, 32768, "<html><body><b>IP:</b><br>%s<br><br><b>REQLINE (%p, %ld):</b><br>%s<br><br><b>HEADERS (%p, %ld):</b><br>%s</body></html>", str, &REQLINE, strlen(REQLINE), REQLINE, &HEADERS, strlen(HEADERS), HEADERS); //, &REQBODY, strlen(REQBODY), REQBODY);
        char *formatted_html = strreplace(html, "\n", "<br>");

        // generate and send response http packet
        char res[32768];
        snprintf(res, 32768, "HTTP/1.0 200 OK\nServer: snadol 0.1\n\n%s", formatted_html);
        write(conn_socket, res, strlen(res));

        // close socket
        close(conn_socket);
    }
}

int main()
{
    // create server struct with port
    int port = 9000;
    Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, port, 255, run);

    // run funciton as defined in struct
    printf("Listening on port %d\n", port);
    server.run(&server);
}