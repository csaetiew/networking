/* import statements referenced from
   http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoClient.c
*/
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "package.h"    /* for package struct*/

int main(int argc, char *argv[]) {
     /*
       Variables names/structs referenced from
       http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c
    */
    int sock;                       /* Socket descriptor */
    struct sockaddr_in servAddr;    /* Server address */
    unsigned short portNum;         /* Server port */
    char *servIP;                   /* Server IP address (dotted quad) */
    char *message;                  /* String to send to server */
    int bytesRcvd, totalBytesRcvd;  /* Bytes read in single recv() 
                                        and total bytes read */
   
   
    char *mode;                      /* Download or Upload mode*/

    if (argc != 5 && argc != 4)    /* Test for correct number of arguments */
    {
       printf("The number of arguement is incorrect, it must be in the for of ./ttweetcl -u/d <Server IP> <Server Port> <\"message\">\n");
       exit(1);
    } 
    
    mode = argv[1] + 1;               /* First arg: is mode to upload or download*/

    /* 
       Tests for the number of arguments in the 
       case for uploading mode and downloading mode.
    */
    if (*mode == 'u' && argc != 5 ) {
       printf("The number of arguement is incorrect, it must be in the for of ./ttweetcl -u <Server IP> <Server Port> <\"message\">\n");
       exit(1); 
    } else if (*mode == 'd' && argc != 4) {
       printf("The number of arguement is incorrect, it must be in the for of ./ttweetcl -d <Server IP> <Server Port>\n");
       exit(1); 
    } else if (*mode != 'u' && *mode != 'd') {
        printf("A mode, -d or -u, is missing\n");
       exit(1); 
    }
    

    servIP = argv[2];        /* Second arg: server IP address (dotted quad) */
    portNum = atoi(argv[3]); /* Third arg: server port number*/
    if (*mode == 'u') {      /* Only sets message to the input message if upload mode*/
        message = argv[4];   /* Second arg: Message to send*/

        if (strlen(message) > 150) { /*Sends error to console if lenth of message > 150 char*/
            printf("The message exceeds the limit of 150. It has %d characters\n", strlen(message));
            exit(1);
        }
    } else {
        message = "NULL";   /* message is NULL if in downloading mode*/
    }


    /* 
       socket creation, socket(), server address structure
       initialization, and connection, connect() referenced from
       http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoClient.c
    */
   /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        printf("Failed to create socket: socket()");

    /* Construct the server address structure */
    memset(&servAddr, 0, sizeof(servAddr));         /* Zero out structure */
    servAddr.sin_family      = AF_INET;             /* Internet address family */
    servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    servAddr.sin_port        = htons(portNum);      /* Server port */

    /* Establish the connection to the server server */
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        printf("Server not responding\n");
        exit(1);
    }

    /* differentiating the two modes */
    if (*mode == 'u') {
        
        /*
           Declares new package, clear it, and then
           set the mode 'u', the size of the message
           and the message
        */
        Package pack;
        memset(&pack, 0, sizeof(pack));
        memcpy(&pack.mode, mode, sizeof(char));
        pack.size = strlen(message);
        memcpy(&pack.message, message, strlen(message));


    /* Send the string to the server and prints confirmation to console*/
        if (send(sock, &pack, sizeof(Package), 0) != sizeof(Package)) {
            printf("Failed to send package to server");
        }
        printf("Message Upload Successful\n");

    } else if(*mode == 'd') {
        /* Declare new package clear it, then set the mode to 'd' */
        Package pack;
        memset(&pack, 0, sizeof(pack));
        memcpy(&pack.mode, mode, sizeof(char));

        /* Send the string to the server and prints confirmation to console*/
        if (send(sock, &pack, sizeof(Package), 0) != sizeof(Package))
            printf("Failed to send package to server");

        /* Receive package from server*/
        if (recv(sock, &pack, sizeof(Package), 0)<= 0)
            printf("Failed to retrieve packet from server");

        /* prints the message received*/
        printf("Recieved %s\n", pack.message);
    }
    /* Closes socket */
    close(sock);
    exit(0);
}
