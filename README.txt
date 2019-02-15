Chawalit Saetiew csaetiew3@gatech.edu
CS3251-A Computer Networking I, 2/14/2019, Programming Assignment 1

File:
ttweetsrv.c: contains code for the server side
ttweetcl.c: contains code for the client side
package.h: contains the struct for package which is the data type that is sent over the network.
README.txt: contains personal/class/project information
Sample.txt: contains inputs and outputs of the commandline based on pdf



How to Run:
Compile server using: gcc -o ttweetsrv ttweetsrv.c
Compile client using: gcc -o ttweetcl ttweetcl.c 

To run the server use: ./ttweetsrv <Server Port>
For example: ./ttweetsrv 14000

To upload with client use: ./ttweetcl -u <Server IP> <Server Port> "message"
For example: ./ttweetcl -u 127.0.0.1 14000 "This is a test"

To download with client use: ./ttweetcl -d <Server IP> <Server Port>
For example: ./ttweetcl -d 127.0.0.1 14000


Protocol:
The client creates a socket based on IP protocol family, stream,
and TCP. The client obtains the IP address and port number of the server and connects
to the server with the socket.

When sending or receiving messages, the messages are wrapped in a Package(a twist to packet)
structure. The Packet structure contains 3 variables, char mode(), int size, and char message[150].
The mode is set by the client and is mainly used by the server to determin whether to save the message,
or to send the saved message. The size variable is to determine how many bits of the message contains a
valid character and is set and used by both the client and the server to send and parse the message.
message having a set 150 character, even if only parts of the array contains actual 
information, makes communication more structured, so there isn't a variable message char array length.
 
The client has 2 modes, upload mode(-u) and download mode(-d).

When in upload mode, the client sets the size and mode of the package. Then it puts the message in and sends the package to the server
The client checks that the amount of sent bytes match the size of the package which is 160 bytes.

When in download mode, the client sends a package with only the mode set to d and checks that the
amount of sent bytes match the size of the package which is 160 bytes.
The client then receives the package sent by the server and retrieves the message and prints it.

The server has 2 modes, upload mode(-u) and download mode(-d).

When in upload mode, the server has received a package that the client has uploaded. The 
server clears the storage variable and copys the message inside the package based on the size of
the package using memcpy. It then sets the size to the current stored message size and prints
a confirmation.

When in download mode, the server has received a request for download from the client. The server
creates a package, sets the mode to d, the size, and the message from the storage variable.
The server then sends the package back to the client and prints a confirmation.

Connection with the client is closed by the server.

References:
http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoClient.c
http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c





