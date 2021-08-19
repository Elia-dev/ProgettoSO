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
#include "constHeader.h"

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
    return pid;
}

int findPid(char *name)
{
    char result[16];
    int pid;
    FILE *fp = fopen(PIDPATH, "r");
    if(fp != NULL)
    {
        printf("ho aperto il file\n");
    }

    do
    {
        fgets(result, 15, fp);
        printf("%s\n", result);
        //sleep(1);
    }
    while(result[0] != name[0] || result[1] != name[1]);  // Legge i vari pid finché non trova quello richiesto

    char *substring;
    strtok(result, " ");
    substring = strtok(NULL, " ");
    pid = atoi(substring);
    fclose(fp);
    //printf("PID trovato: %d\n", pid);
    return pid;
}

int main()
{
    int p1, p2, p3;
    int sommaP1, sommaP2, sommaP3;
    int tmp;
    int pidFailManager;
    int pidWatchDog;
    FILE *fpOutput, *fpSysLog;
    generatePid();
    createSocket();
    //printf("socket creato\n");
    fpSysLog = fopen(SYSLOG, "w");
    fpOutput = fopen(OUTPUT, "a"); // Apertura del file
    printf("DF:  PRONTO\n");
    do
    {

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        /*
        printf("Client connesso\n");
        sommaP1 = readProcess(p1);
        printf("Numero processo: %d\t somma: %d", p1, sommaP1);
        close(clientFd);*/
        //printf("Client connesso: ");
        read(clientFd, &tmp, sizeof(tmp));
        p1 = ntohl(tmp); // Converte da network byte order a int
        //printf("%d\t", p1);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP1 = ntohl(tmp);
        //printf("valore ricevuto: %d\n", sommaP1);
        close(clientFd);
        //printf("\n");

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        //printf("Client connesso: ");
        read(clientFd, &tmp, sizeof(tmp));
        p2 = ntohl(tmp); // Converte da network byte order a int
        //printf("%d\t", p2);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP2 = ntohl(tmp);
        //printf("valore ricevuto: %d\n", sommaP2);
        close(clientFd);
        //printf("\n");

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        //printf("Client connesso: ");
        read(clientFd, &tmp, sizeof(tmp));
        p3 = ntohl(tmp); // Converte da network byte order a int
        //printf("%d\t", p3);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP3 = ntohl(tmp);
        //printf("valore ricevuto: %d\n", sommaP3);
        close(clientFd);
        //printf("\n\n\n\n\n");
        if(sommaP1 > 0 && sommaP2 > 0 && sommaP3 > 0)
        {
            printf("HO RICEVUTO LA SOMMA\n");
            fprintf(fpOutput, "%d %d %d\n", sommaP1, sommaP2, sommaP3); // Scrivo su file i risultati dei processi
            //manda un segnale I_AM_ALIVE al watchdog

            if(sommaP1 == sommaP2 || sommaP1 == sommaP3 || sommaP2 == sommaP3)   // Eseguo il voto di maggioranza
            {

                fprintf(fpSysLog, "SUCCESSO\n");
                printf("HO SCRITTO SUCCESSO\n");
                pidWatchDog = findPid("WD");
                printf("PID PER IL WATCHDOG IN SUCCESSO: %d", pidWatchDog);
                kill(pidWatchDog, SIGUSR2); // Manda I_AM_ALIVE
            }
            else
            {
                fprintf(fpSysLog, "FALLIMENTO\n");
                printf("HO SCRITTO FALLIMENTO\n");
                pidWatchDog = findPid("WD");
                printf("PID PER IL WATCHDOG IN FALLIMENTO: %d", pidWatchDog);
                fclose(fpOutput);
                fclose(fpSysLog);
                kill(pidWatchDog, SIGUSR2);
                pidFailManager = findPid("FM");

                kill(pidFailManager, SIGUSR1); //manda un segnale SIGUSR1 a failureManager
            }
        }

    }
    while(sommaP1 > 0 || sommaP2 > 0 || sommaP3 > 0);
    close(serverFd);
    fclose(fpSysLog);
    fclose(fpOutput);
    unlink(SOCKETDF);

    return 0;
}
