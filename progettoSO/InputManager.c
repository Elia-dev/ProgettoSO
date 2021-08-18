#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep(), unlink()...
#include <string.h>
#include <fcntl.h> // For open() constants
#include <sys/socket.h>
#include <sys/un.h> // For AF_UNIX sockets
#include <signal.h>
#include "constHeader.h"


int fd;
int serverFd;
int clientFd;
int fd3;

int openSocket()
{

    int serverLen, clientLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    struct sockaddr_un clientUNIXAddress; //Client address
    struct sockaddr* clientSockAddrPtr;//Ptr to client address

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; //Set domain type
    strcpy (serverUNIXAddress.sun_path, SOCKET); // Set name
    unlink (SOCKET); // Remove file if it already exists
    bind (serverFd, serverSockAddrPtr, serverLen); //Create file
    listen (serverFd, 1); // Maximum pending connection length

    clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
}

void sendToSocket(char *message)
{
    write (clientFd, message, strlen (message) + 1); /* Write first line */
}

int openPipe()
{
    int fd;

    do    //Keep trying to open the file until successful
    {
        fd = open (PIPE, O_WRONLY); //Open named pipe for writing
        if (fd == -1)
        {
            printf("IM: Error opening pipe\n");
            sleep (1); // Try again in 1 second
        }
    }
    while (fd == -1);

    return fd;
}

void sendToPipe(char *message)
{
    write (fd, message, strlen(message) + 1); //Write message down pipe
}

void openSharedFile()
{
    fd3 = open(FILEPATH, O_CREAT|O_WRONLY|O_TRUNC, 0777);
}

void sendToSharedFile(char *message)
{
    /* for(int i = 0; i < strlen(message); i++) {
         printf("INT: %d CHAR: %c", message[i], message[i]);
     }*/
    //printf("\n");
    //fprintf(fileP3, "%s", message);
    write(fd3, message, strlen(message) + 1);
}

int generatePid()   //metodo che genera un file contenente il PID di questo processo
{
    FILE *fp;
    fp = fopen(PIDPATH, "w");
    int pid = getpid();
    fprintf(fp, "IM: %d\n", pid);
    fclose(fp);
}

int main()
{
    generatePid();
    //char *path = "../dataset2.csv";
    char car;
    FILE *fp;
    int dimRiga = 0;

    fp = fopen(PATHDATASET, "r"); // Apertura del file in sola lettura
    //printf("file dataset aperto\n");
    fd = openPipe();
    //printf("pipe aperto\n");
    serverFd = openSocket();
    //printf("socket p2 aperto\n");
    openSharedFile();
    //printf("file condiviso aperto\n");
    printf("IM: PRONTO\n");
    // Scarto la prima riga leggendola a vuoto
    while(car != '\n')   // Legge la prima riga
    {
        car = fgetc(fp); // Leggendo carattere per carattere
        //printf("%c", car);
    }

    car = 'a'; // Reset del carattere di controllo

    // Conta dimensioni righe
    while(car != '\n')
    {
        car = fgetc(fp);
        dimRiga++; // Conta di quanti caratteri è lunga la seconda riga, si assume che le righe abbiano la stessa lunghezza
    }

    fseek(fp, -dimRiga, 1); // Mi riposiziono all'inizio della seconda riga
    dimRiga += 3;
    char buffer[dimRiga];

    while(fgets(buffer, dimRiga, fp))  // Scorro tutto il file
    {
        //printf("STRINGA LETTA DA FILE: %s\n\n", buffer);
        sendToPipe(buffer);
        sendToSocket(buffer);
        sendToSharedFile(buffer);
        sleep(1);
    }

    fclose(fp); // Chiude il file dataset.csv
    close(fd); // Chiude il file descriptor della pipe
    close(clientFd); //Close the client
    //printf("Client chiuso\n");
    close(serverFd); //Close the socket
    //printf("Server chiuso\n");
    close(fd3);
    unlink(SOCKET);
    printf("IM: TERMINATO\n");
    // KILL SIGNAL TO FAILURE MANAGER
    return 0;
}
