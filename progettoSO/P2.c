#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/un.h> // For AF_UNIX sockets

#define DEFAULT_PROTOCOL 0
#define SOCKET "socketP2"
#define SOCKETDF "socketDecisionFunction"

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
/// da influenzare con la funzione random_failure
	int charSum = 0;
	for(int i = strlen(str) - 2; i >= 0; i--) { // -2 perchè l'ultimo carattere è '\n'
		charSum += str[i] * (str[i] != 44); // 44 = virgola
	}
	return charSum;
}

int random_failure(int attivo)
{
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale ad 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 20;
    }
    else return 0;
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
		charSum += random_failure(0);
		printf("Somma: %d: \n", charSum);
		//sendToDecisionFunction(charSum);
    }
    close (clientFd); // Close the socket
    printf("Socket chiusa\n");
    return 0;
}
