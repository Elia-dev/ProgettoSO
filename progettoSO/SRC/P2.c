#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <time.h>
#include "ConstHeader.h"

int openInputManagerSocket()
{
    int clientFd, serverLen, connection;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy (serverUNIXAddress.sun_path, SOCKET);
    do
    {
        connection = connect (clientFd, serverSockAddrPtr, serverLen);
        if (connection == -1)
        {
            printf("P2->InputManager: Retrying connection in 1 sec\n");
            sleep (1);
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
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy (serverUNIXAddress.sun_path, SOCKETDF);
    do
    {
        connection = connect (clientFd, serverSockAddrPtr, serverLen);
        if (connection == -1)
        {
            printf("P2->DecisionFunction: Retrying connection in 1 sec\n");
            sleep (1);
        }
    }
    while (connection == -1);

    return clientFd;
}

int readLine (int fd, char *str)
{
    int n;
    do
    {
        n = read(fd, str, 1);
    }
    while (n > 0 && *str++ != '\0');
    return (n > 0); // Ritorna falso se arriva alla fine dell'input
}

int sum(char *str)
{
    int charSum = 0;
    for(int i = strlen(str) - 2; i >= 0; i--)   // -2 perchè l'ultimo carattere è '\n'
    {
        charSum += str[i] * (str[i] != 44); // 44 = virgola, ignora la virgola
    }
    return charSum;
}

int random_failure(int attivo)
{
    srand(time(NULL) + 2);
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale a 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 20;
    }
    else return 0;
}

void sendToDecisionFunction(int clientDecisionFunction, int sum)
{
    int tmp;

    tmp = htonl(sum); // Converte sum in network Byte Order
    write(clientDecisionFunction, &tmp, sizeof(tmp));
}

int main()
{
    int clientFd;
    int clientDecisionFunction;
    char str[700]; // 700 perchè le righe sono circa 550 caratteri e sennò va fuori memoria e crasha
    int charSum = 0;
    savePidOnFile("P2", getpid());

    clientFd = openInputManagerSocket();
    printf("P2: READY\n");
    int count = 0;
    while(readLine(clientFd, str) > 0)  // Legge finché trova qualcosa da leggere
    {
        charSum = sum(str);
        charSum += random_failure(MODEXEC);
        clientDecisionFunction = openDecisionFunctionSocket();
        sendToDecisionFunction(clientDecisionFunction, charSum);
        close(clientDecisionFunction);
        sleep(1);
    }

    clientDecisionFunction = openDecisionFunctionSocket();
    sendToDecisionFunction(clientDecisionFunction, -1); // Avviso Decision Function che ho terminato
    close(clientDecisionFunction);
    close (clientFd);
    printf("P2: TERMINATED\n");
    return 0;
}
