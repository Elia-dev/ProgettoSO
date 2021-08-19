#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For write(), sleep(), read()
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h> // For AF_UNIX sockets
#include <arpa/inet.h> // For order byte network
#include <time.h>
#include "constHeader.h"

int openInputManagerSocket()
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
            printf("P2->InputManager: Retrying connection in 1 sec\n");
            sleep (1); // Wait and then try again
        }
    }
    while (connection == -1);

    return clientFd;
}

int openDecisionFunctionSocket()
{
    int clientFd, serverLen, connection;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Server domain
    strcpy (serverUNIXAddress.sun_path, SOCKETDF);// Server name
    do   // Loop until a connection is made with the server
    {
        connection = connect (clientFd, serverSockAddrPtr, serverLen);
        if (connection == -1) // Se la connessione fallisce
        {
            printf("P2->DecisionFunction: Retrying connection in 1 sec\n");
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

int sum(char *str)
{
    int charSum = 0;
    for(int i = strlen(str) - 2; i >= 0; i--)   // -2 perchè l'ultimo carattere è '\n'
    {
        charSum += str[i] * (str[i] != 44); // 44 = virgola
    }
    return charSum;
}

int random_failure(int attivo)
{
    srand(time(0));
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale ad 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 20;
    }
    else return 0;
}

void sendToDecisionFunction(int clientDecisionFunction, int sum)
{
    int tmp;
    if(sum >= 0)
    {
        tmp = htonl(2); // Converte sum in network Byte Order
        write(clientDecisionFunction, &tmp, sizeof(tmp));
        tmp = htonl(sum); // Converte sum in network Byte Order
        write(clientDecisionFunction, &tmp, sizeof(tmp));
    }
    else
    {
        tmp = htonl(2); // Converte sum in network Byte Order
        write(clientDecisionFunction, &tmp, sizeof(tmp));
        tmp = htonl(-1); // Converte sum in network Byte Order
        write(clientDecisionFunction, &tmp, sizeof(tmp));
    }

    //write (clientFd, str2, strlen(str2) + 1); /* Write first line */
}

int generatePid()   //metodo che genera un file contenente il PID di questo processo
{
    FILE *fp = fopen(PIDPATH, "a");
    int pid = getpid();
    fprintf(fp, "P2: %d\n", pid);
    fclose(fp);
    return pid;
}

int main()
{
    int clientFd;
    int clientDecisionFunction;
    char str[700]; // 700 perchè le righe sono grosse circa 550 e sennò va fuori memoria e crasha
    int charSum = 0;
    generatePid();

    clientFd = openInputManagerSocket();
    printf("P2: PRONTO\n");
    int count = 0;
    while(readLine(clientFd, str))
    {
        //printf("Stringa ricevuta: %s\n", str);
        charSum = sum(str);
        charSum += random_failure(MODEXEC);
        clientDecisionFunction = openDecisionFunctionSocket();
        printf("P2->DF: SOCKET APERTO\n");
        //printf("Somma: %d: \n", charSum);
        sendToDecisionFunction(clientDecisionFunction, charSum);
        close(clientDecisionFunction);
        printf("P2->DF: SOCKET CHIUSO\n");
        sleep(1);
    }
    clientDecisionFunction = openDecisionFunctionSocket();
    sendToDecisionFunction(clientDecisionFunction, -1);
    close(clientDecisionFunction);
    close (clientFd); // Close the socket
    printf("P2: TERMINATO\n");
    return 0;
}
