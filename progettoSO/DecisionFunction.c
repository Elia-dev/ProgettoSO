#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h> // For AF_UNIX sockets
#include <arpa/inet.h>

#define DEFAULT_PROTOCOL 0
#define SOCKETDF "socketDF"
#define OUTPUT "voted_output"
#define SYSLOG "system_log"
#define PIDPATH "filePid"

int serverFd;
int clientFd;
int serverLen, clientLen;
struct sockaddr_un serverUNIXAddress; // Server address
struct sockaddr* serverSockAddrPtr; // Ptr to server address
struct sockaddr_un clientUNIXAddress; // Client address
struct sockaddr* clientSockAddrPtr; // Ptr to client address

void createSocket()
{
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Set domain type
    strcpy (serverUNIXAddress.sun_path, SOCKETDF); // Set name
    unlink (SOCKETDF); // Remove file if it already exists
    bind (serverFd, serverSockAddrPtr, serverLen); // Create file
    listen (serverFd, 3); // Maximum pending connection length
}
/*
int readProcess(int *process)
{
    int tmp;
    read(clientFd, &process, sizeof(process));
    process = ntohl(process); // Converte da network byte order a int
    read(clientFd, &tmp, sizeof(tmp));
    return tmp = ntohl(tmp);
}
*/

int generatePid()   //metodo che genera un file contenente il PID di questo processo
{
    FILE *fp = fopen(PIDPATH, "a");
    int pid = getpid();
    fprintf(fp, "DF: %d\n", pid);
    fclose(fp);
}

int main()
{
    int p1, p2, p3;
    int sommaP1, sommaP2, sommaP3;
    int tmp;
    FILE *fpOutput, *fpSysLog;
    generatePid();
    createSocket();
    printf("socket creato\n");
    fpOutput = fopen(OUTPUT, "w"); // Apertura del file
    fpSysLog = fopen(SYSLOG, "w");
    // bind (serverFd, serverSockAddrPtr, serverLen); //Create file
    // listen (serverFd, 3); // Maximum pending connection length
    do
    {

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        /*
        printf("Client connesso\n");
        sommaP1 = readProcess(p1);
        printf("Numero processo: %d\t somma: %d", p1, sommaP1);
        close(clientFd);*/
        printf("Client connesso: ");
        read(clientFd, &tmp, sizeof(tmp));
        p1 = ntohl(tmp); // Converte da network byte order a int
        printf("%d\t", p1);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP1 = ntohl(tmp);
        printf("valore ricevuto: %d\n", sommaP1);
        close(clientFd);
        printf("\n");

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        printf("Client connesso: ");
        read(clientFd, &tmp, sizeof(tmp));
        p2 = ntohl(tmp); // Converte da network byte order a int
        printf("%d\t", p2);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP2 = ntohl(tmp);
        printf("valore ricevuto: %d\n", sommaP2);
        close(clientFd);
        printf("\n");

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        printf("Client connesso: ");
        read(clientFd, &tmp, sizeof(tmp));
        p3 = ntohl(tmp); // Converte da network byte order a int
        printf("%d\t", p3);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP3 = ntohl(tmp);
        printf("valore ricevuto: %d\n", sommaP3);
        close(clientFd);
        printf("\n\n\n\n\n");
        if(sommaP1 > 0 && sommaP2 > 0 && sommaP3 > 0)
        {
            fprintf(fpOutput, "%d %d %d\n", sommaP1, sommaP2, sommaP3); // Scrivo su file i risultati dei processi
            //manda un segnale I_AM_ALIVE al watchdog
            if(sommaP1 == sommaP2 || sommaP1 == sommaP3 || sommaP2 == sommaP3)   // Eseguo il voto di maggioranza
            {
                fprintf(fpSysLog, "SUCCESSO\n");
            }
            else
            {
                fprintf(fpSysLog, "FALLIMENTO\n");

             //   kill(pidFailManager, SIGUSR1); //manda un segnale SIGUSR1 a failureManager
            }
        }


    }
    while(sommaP1 > 0 || sommaP2 > 0 || sommaP3 > 0);

    close(serverFd);
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
