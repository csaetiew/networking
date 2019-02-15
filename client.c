#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "package.h" /* for package struct*/



void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int sendMessage(int sock, Package* message);

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in servAddr; /* Echo server address */
    unsigned short portNum;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *message;                /* String to send to echo server */
    char buffer[sizeof(Package)];     /* Buffer for echo string */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */
    char *mode;                       /* Download or Upload mode*/

    if (argc != 5 && argc != 4)    /* Test for correct number of arguments */
    {
       printf("The number of arguement is incorrect, it must be in the for of ./ttweetcl -u/d <Server IP> <Server Port> <\"message\">\n");
       exit(1);
    } 
    
    mode = argv[1] + 1;               /* First arg: is mode to upload or download*/

    if (*mode == 'u' && argc != 5 ) {
       printf("The number of arguement is incorrect, it must be in the for of ./ttweetcl -u <Server IP> <Server Port> <\"message\">\n");
       exit(1); 
    } else if (*mode == 'd' && argc != 4) {
       printf("The number of arguement is incorrect, it must be in the for of ./ttweetcl -d <Server IP> <Server Port>\n");
       exit(1); 
    }
    
    

    mode = argv[1] + 1;               /* First arg: is mode to upload or download*/
    servIP = argv[2];             /* Second arg: server IP address (dotted quad) */
    portNum = atoi(argv[3]); /* Third arg: server port number*/
    if (*mode == 'u') {
        message = argv[4];         /* Second arg: Message to send*/
        if (strlen(message) > 150) {
            printf("The message exceeds the limit of 150. It has %d characters\n", strlen(message));
            exit(1);
        }
    } else {
        message = "NULL";
    }
    Package pack;

    memset(&pack, 0, sizeof(pack));
    memcpy(&pack.mode, mode, sizeof(char));
    pack.size = strlen(message);
    memcpy(&pack.message, message, strlen(message));

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
    servAddr.sin_family      = AF_INET;             /* Internet address family */
    servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    servAddr.sin_port        = htons(portNum); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("connect() failed");

    //division point
    // printf(mode);
    // printf("%c", pack.mode);
    int messageLength = strlen(message);          /* Determine input length */
    if (*mode == 'u') {
        // sendMessage(sock, message);
       int messageLength = sizeof(Package);          /* Determine input length */

    /* Send the string to the server */
        
        if (send(sock, &pack, sizeof(Package), 0) != sizeof(Package))
            DieWithError("send() failed");
        printf("Message Upload Successful");
        close(sock);
        exit(0);
    } else if(*mode == 'd') {
    /* Send the string to the server */
        if (send(sock, &pack, sizeof(Package), 0) != sizeof(Package))
            DieWithError("send() failed");


        if ((bytesRcvd = recv(sock, buffer, sizeof(Package), 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        Package *temp = buffer;
        printf("Recieved %s", temp->message);
        close(sock);
        exit(0);
    }
    
    close(sock);
    exit(0);
}
