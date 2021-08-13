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
    serverUNIXAddress.sun_family = AF_UNIX; // Server domain 
    strcpy (serverUNIXAddress.sun_path, SOCKET);// Server name
    do   // Loop until a connection is made with the server 
    {
        connection = connect (clientFd, serverSockAddrPtr, serverLen);
        if (connection == -1) // Se la connessione fallisce
        {
            printf("Retrying connection in 1 sec\n");
            sleep (1); // Wait and then try again
        }
    }
    while (connection == -1);

    return clientFd;
}

int readLine (int fd, char *str)
{
    int n;
    do // Read characters until '\0' or end-of-input
    {
        n = read(fd, str, 1); 
    }
    while (n > 0 && *str++ != '\0');
    return (n > 0); // Return false if end-of-input
}

int sum(char *str) {
	int charSum = 0;
	for(int i = strlen(str) - 2; i >= 0; i--) {
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
    close (clientFd); // Close the socket
    printf("Socket chiusa\n");
    return 0;
}
