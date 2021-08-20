#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
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
            printf("P1: Retrying socket connection in 1 sec\n");
            sleep (1);
        }
    }
    while (connection == -1);

    return clientFd;
}

int readLine (int fd, char *str) // Legge una riga dal file passato
{
    int n;
    do
    {
        n = read(fd, str, 1);
    }
    while (n > 0 && *str++ != '\0');
    return (n > 0); // Ritorna falso se arriva alla fine dell'input
}

void createPipe() // Creazione pipe per comunicare con InputManager
{
    unlink(PIPE);
    mknod (PIPE, S_IFIFO, 0);
    chmod (PIPE, 0660);
}

int sum(char *token)
{
    int charSum = 0;
    for(int i = 0; i < strlen(token); i++)
    {
        charSum += token[i]; // Somma tutti i caratteri provenienti dal token
    }
    return charSum;
}

int random_failure(int attivo)
{
    srand(time(NULL));
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale ad 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 10;
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
    int fd;
    char str[700]; // 700 perchè le righe sono circa 550 caratteri e sennò va fuori memoria e crasha
    char *token;
    int charSum;
    int clientFd;
    savePidOnFile("P1", getpid());
    createPipe();

    do {
        fd = open (PIPE, O_RDONLY);
        if(fd == -1) {
            printf("P1: error opening pipe\n");
        }
    }while(fd == -1);

    printf("P1: READY\n");

    while(readLine (fd, str) > 0) // Legge finché trova qualcosa da leggere
    {
        charSum = 0;
        str[strlen(str) - 1] = '\0'; // Alla fine di ogni riga viene sovrascritto il carattere "end of trans. block" (valore ascii 23) con '\0'
        token = strtok(str, ",");
        // Tramite strtok la riga viene spezzata ogni volta che si trova una virgola e il suo valore intero sommato
        while( token != NULL )
        {
            charSum += sum(token);
            token = strtok(NULL, ",");
        }
        charSum += random_failure(MODEXEC);

        clientFd = openSocket();
        sendToDecisionFunction(clientFd, charSum);
        close(clientFd);
        sleep(1); // Utilizzato per sincronizzare decisionFunction con P1, P2 e P3
    }

    clientFd = openSocket();
    sendToDecisionFunction(clientFd, -1); // Avviso Decision Function che ho terminato
    close(clientFd);
    close (fd);
    unlink(PIPE);
    printf("P1: TERMINATED\n");
    return 0;
}
