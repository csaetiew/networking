/* import statements referenced from
   http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c
*/
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdio.h>      /* for perror() */
#include "package.h"    /* for package struct*/


/* creating the storage for messages
   The initial size is 13 for 
   "Empty Message"
*/
char storage[150];
int size = 13;
char* emptyMessage = "Empty Message";

int main(int argc, char *argv[]) {

    /*
       Variables names/structs referenced from
       http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c
    */
    int servSock;                   /* Socket descriptor for server */
    int clntSock;                   /* Socket descriptor for client */
    struct sockaddr_in servAddr;    /* Local address */
    struct sockaddr_in clntAddr;    /* Client address */
    unsigned short servPort;        /* Server port */
    unsigned int clntLen;           /* Length of client address data structure */

    /* clearing and storing "Empty Message"
    */
    memset(storage, 0, sizeof(storage));   
    memcpy(storage, emptyMessage, sizeof(char) * 13);


    if (argc != 2)     /* Test for correct number of arguments */
    {
        printf("The number of arguement is incorrect, it must be in the for of ./ttweetsrv <Server Port>\n");
        exit(1);
    }

    servPort = atoi(argv[1]);  /* First arg:  local port */

    /*
       Creating socket(), bind(), listen() referenced from
       http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c
    */
    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Failed to create socket: socket()");
    }

    /* Construct local address structure */
    memset(&servAddr, 0, sizeof(servAddr));         /* Zero out structure */
    servAddr.sin_family = AF_INET;                  /* Internet address family */
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any incoming interface */
    servAddr.sin_port = htons(servPort);            /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
        printf("Failed to bind Socket: bind()");
    }
    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, 1) < 0){
        printf("Failed to wait for request: listen()");
    }

    while (1) {

        /*
           accept() and recv() referenced from 
           http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c
        */
        /* Set the size of the in-out parameter */
        clntLen = sizeof(clntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0)
            printf("Failed to accept client");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

        /*
           Delcare a new Package and clears it.
        */    
        Package pack;
        memset(&pack, 0, sizeof(Package));

        /* Receive package from client */
        if ((recv(clntSock, &pack, sizeof(Package), 0)) < 0)
            printf("Failed to retrieve packet from client");

        /* differentiating the two modes */
        if (pack.mode == 'd') {

            /*
               Server is in downloading mode.
               Clears the package and store the mode, size
               and the content of the stored message.
            */
            memset(&pack, 0, sizeof(pack));
            pack.mode = 'd';
            pack.size = size;
            memcpy(&pack.message, storage, size);
            
            /*
               Sends the package back to the client
               and prints confirmation to console.
            */
            if (send(clntSock, &pack, sizeof(Package), 0) != sizeof(Package))
                printf("Failed to send package to client");
            printf("Successfully sent %s to client\n", storage);
            continue;

        } else if (pack.mode == 'u') {
            /*
               Server is in uploading mode
               Clears the storage and stores
               the message in the storage.
               Sets size to the size of the message,
               and prints confirmation to console
            */
            memset(storage, 0, sizeof(storage));
            memcpy(storage, &pack.message, pack.size);
            size = pack.size;
            printf("Successfully stored %s\n", storage);

        }
        memset(&pack, 0, sizeof(Package));

        close(clntSock);    /* Close client socket */
    }
    /* NOT REACHED */
}
