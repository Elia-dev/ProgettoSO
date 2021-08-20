#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "ConstHeader.h"

int serverFd;
int clientFd;

int openSocket() // Apertura del socket per comunicare con P2
{
    int serverLen, clientLen;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    struct sockaddr_un clientUNIXAddress;
    struct sockaddr* clientSockAddrPtr;

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy (serverUNIXAddress.sun_path, SOCKET);
    unlink (SOCKET);
    bind (serverFd, serverSockAddrPtr, serverLen);
    listen (serverFd, 1);

    clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
}

void sendToSocket(char *message)
{
    write (clientFd, message, strlen (message) + 1);
}

int openPipe() // Apertura della pipe per comunicare con P1
{
    int fd;

    do
    {
        fd = open (PIPE, O_WRONLY);
        if (fd == -1)
        {
            printf("IM: Error opening pipe\n");
            sleep (1);
        }
    }
    while (fd == -1);

    return fd;
}

void sendToPipe(int fd, char *message)
{
    write (fd, message, strlen(message) + 1);
}

int openSharedFile() // Apertura del file condiviso per comunicare con P3
{
    int fd;
    do{
        fd = open(FILEPATH, O_CREAT|O_WRONLY|O_TRUNC, 0777);
        if (fd == -1)
        {
            printf("IM: Error opening shared file\n");
            sleep (1);
        }
    }while(fd == -1);

    return fd;
}

void sendToSharedFile(int fd, char *message)
{
    write(fd, message, strlen(message) + 1);
}

int main()
{
    savePidOnFile("IM", getpid());
    char car;
    FILE *fp;
    int dimRiga = 0;
    int fd, fd3;

    do {
        fp = fopen(PATHDATASET, "r");
        if(fp == NULL) {
            printf("IM: error opening dataset file\n");
            sleep(1);
        }
    }while(fp == NULL);

    fd = openPipe();
    serverFd = openSocket();
    fd3 = openSharedFile();
    printf("IM: READY\n");

    // Scarto la prima riga leggendola a vuoto
    do {
        car = fgetc(fp); // Leggendo carattere per carattere
    }while(car != '\n');

    // Conta dimensioni righe
    do{
        car = fgetc(fp);
        dimRiga++; // Conta di quanti caratteri è lunga la seconda riga, essendo un file di log si assume che le righe abbiano circa la stessa lunghezza
    }

    while(car != '\n');

    fseek(fp, -dimRiga, 1); // Riposizionamento all'inizio della seconda riga
    dimRiga += 50; // Assumendo che le righe possano variare la loro lunghezza di qualche carattere, per sicurezza leggiamo 50 caratteri in più
    char buffer[dimRiga];

    while(fgets(buffer, dimRiga, fp))  // Scorro tutto il file
    {
        sendToPipe(fd, buffer);
        sendToSocket(buffer);
        sendToSharedFile(fd3, buffer);
        sleep(1);
    }

    fclose(fp); // Chiude il file dataset.csv
    close(fd); // Chiude il file descriptor della pipe
    close(clientFd); // Chiude il client
    close(serverFd); // Chiude la socket
    close(fd3); // Chiude il file condiviso
    unlink(SOCKET);
    printf("IM: TERMINATED\n");
    return 0;
}
