#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h> // For AF_UNIX sockets

#define DEFAULT_PROTOCOL 0
#define SOCKETDF "socketDecisionFunction"

int serverFd;
int clientFd;
int serverLen, clientLen;
struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    struct sockaddr_un clientUNIXAddress; //Client address
    struct sockaddr* clientSockAddrPtr;//Ptr to client address
int openSocket()
{


    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; //Set domain type
    strcpy (serverUNIXAddress.sun_path, SOCKETDF); // Set name
    unlink (SOCKETDF); // Remove file if it already exists
    bind (serverFd, serverSockAddrPtr, serverLen); //Create file
    listen (serverFd, 3); // Maximum pending connection length
}

int readLine (int fd, int *str)
{
    int n;
    do // Read characters until '\0' or end-of-input
    {
        n = read(fd, str, 1);
    }
    while (n > 0 && *str++ != '\0');
    return (n > 0); // Return false if end-of-input
}

int main()
{
    int result[2];
    int p1, p2, p3;
    clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
    readLine(clientFd, result);
    printf("%d, %d", result[0], result[1]);
    close(clientFd);
   /* if(str[0] == 1) {
        //p1 =
    }*/
    //chiudi P1
    //apri P2
    //LEGGI DA P2
    // chiudi p2
    //apri P3
    //leggi p3
    // chiudi p3

    return 0;
}
