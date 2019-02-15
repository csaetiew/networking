#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdio.h>  /* for perror() */
#include "package.h"

int sendMessage(int sock, char* message);

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

#define MAXPENDING 5    /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage);  /* Error handling function */
void HandleTCPClient(int clntSocket);   /* TCP client handling function */

char storage[150];
int size = 0;

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in servAddr; /* Local address */
    struct sockaddr_in clntAddr; /* Client address */
    unsigned short servPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    servPort = atoi(argv[1]);  /* First arg:  local port */
    memset(storage, 0, sizeof(storage));



    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr));   /* Zero out structure */
    servAddr.sin_family = AF_INET;                /* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    servAddr.sin_port = htons(servPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    Package* temp;

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(clntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, 
                               &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

        char buffer[sizeof(Package)];        /* Buffer for string */
        memset(&buffer, 0, sizeof(Package));
        int recvMsgSize = 0;                    /* Size of received message */

        /* Receive message from client */

        if ((recvMsgSize = recv(clntSock, buffer, sizeof(Package), 0)) < 0)
            DieWithError("recv() failed");
        temp = buffer;
        printf("Recieved Size is %d\n", recvMsgSize);
        printf("Recieved Mode is %c\n", temp->mode);

        if (temp->mode == 'd') {

            Package pack;
            memset(&pack, 0, sizeof(pack));
            pack.mode = 'd';
            pack.size = size;
            printf("%d", size);
            if (size == 0) {
                char* emptyMessage = "Empty Message";
                memcpy(&pack.message, emptyMessage, sizeof(*emptyMessage) * 13);
            } else {
                memcpy(&pack.message, storage, size);
            }
            printf("returned %s", pack.message);
            if (send(clntSock, &pack, sizeof(Package), 0) != sizeof(Package))
                DieWithError("send() failed");
            continue;
        } else if (temp ->mode == 'u') {
            memset(storage, 0, sizeof(storage));
            memcpy(storage, &temp->message, temp->size);
            size = temp->size;
            printf("saved %s", storage);
        }
        memset(temp, 0, sizeof(Package));

        close(clntSock);    /* Close client socket */
    }
    /* NOT REACHED */
}
