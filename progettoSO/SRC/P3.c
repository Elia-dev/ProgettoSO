#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <time.h>
#include "ConstHeader.h"



int openSocket() // Apertura socket per comunicare con Decision Function
{
    int serverLen, connection, clientFd;
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
            printf("P3: Retrying connection in 1 sec\n");
            sleep (1);
        }
    }
    while (connection == -1);

    return clientFd;
}

int sum(char *str)
{
    int charSum = 0;
    for(int i = 0; i < strlen(str) - 1; i++)     // -1 perchè l'ultimo carattere è '\n'
    {
        charSum += str[i] * (str[i] != 44); // 44 = virgola, ignora la virgola
    }
    return charSum;
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

int random_failure(int attivo)
{
    srand(getpid());
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale ad 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 30;
    }
    else return 0;
}

void sendToDecisionFunction(int clientFd, int sum)
{
    int tmp;

    tmp = htonl(sum); // Converte sum in network Byte Order
    write(clientFd, &tmp, sizeof(tmp));
}

int main()
{
    savePidOnFile("P3", getpid());
    char str[700]; // 700 perchè le righe sono circa 550 caratteri e sennò va fuori memoria e crasha
    int charSum = 0;
    int fd, clientFd;
    do
    {
        fd = open(FILEPATH, O_RDONLY);
        if(fd < 0) {
            printf("P3: Errore durante l'apertura del file condiviso...\n");
            sleep(1);
        }
    }
    while(fd < 0);

    printf("P3: READY\n");

    while(readLine(fd, str) > 0)  // Legge finché trova qualcosa da leggere
    {
        charSum = sum(str);
        charSum += random_failure(MODEXEC);
        clientFd = openSocket();
        sendToDecisionFunction(clientFd, charSum);
        close(clientFd);
        sleep(1);
    }
    clientFd = openSocket();
    sendToDecisionFunction(clientFd, -1); // Avviso Decision Function che ho terminato
    close(clientFd);
    close (fd);
    printf("P3: TERMINATED\n");
    return 0;
}
