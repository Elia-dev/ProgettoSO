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
#define SOCKET "socketP2"


int openSocket()
{
    int clientFd, serverLen, connection;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Server domain /
    strcpy (serverUNIXAddress.sun_path, SOCKET);//Server name/
    do   // Loop until a connection is made with the server /
    {
        connection = connect (clientFd, serverSockAddrPtr, serverLen);
        if (connection == -1) // Se la connessione fallisce
        {
            printf("Connection problem;re-try in 1 sec\n");
            sleep (1); // Wait and then try again /
        }
    }
    while (connection == -1);

    return clientFd;
}

int readLine (int fd, char *str)
{
    // Read a single '\0'-terminated line into str from fd /
    // Return 0 when the end-of-input is reached and 1 otherwise /
    int n;
    do
    {
        // Read characters until '\0' or end-of-input /
        n = read(fd, str, 1);
    }
    while (n > 0 && *str++ != '\0');
    return (n > 0);
    // Return false if end-of-input /
}

/*
int readRecipe (int fd)
{
    char str[200];
    while (readLine (fd, str)) // Read lines until end-of-input /
        printf ("%s\n", str); // Echo line from socket
}
*/

int sum(char *str) {
	int charSum = 0;
	for(int i = strlen(str)-1; i >= 0; i--) {
		charSum += str[i] * (str[i] != 44);
	}
	return charSum;
}

int main()
{
    int clientFd;
    char str[700]; // 700 perchè le righe sono grosse circa 550 e sennò va fuori memoria e crasha
	int charSum = 0;

    clientFd = openSocket();
    printf("CONNESSIONE APERTA\n");
    int count = 0;
    while(readLine(clientFd, str))
    {
		//count ++;
		printf("Stringa ricevuta: %s\n", str);
		charSum = sum(str);
		printf("Somma: %d: ", charSum);
    }
    close (clientFd); // Close the socket /
    printf("Socket chiusa\n");
    return 0;
}
