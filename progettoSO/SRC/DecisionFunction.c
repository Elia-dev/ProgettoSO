#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "ConstHeader.h"

int serverLen;
int clientLen;
int serverFd;
int clientFd;
struct sockaddr* clientSockAddrPtr;

void createSocket() // Creazione della socket per ricevere i dati dall'InputManager
{
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    struct sockaddr_un clientUNIXAddress;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX;
    strcpy (serverUNIXAddress.sun_path, SOCKETDF);
    unlink (SOCKETDF);
    bind (serverFd, serverSockAddrPtr, serverLen);
    listen (serverFd, 3);
}

int readProcess() // Legge la somma mandata dal client
{
    int tmp, sum;

    clientFd = accept (serverFd, clientSockAddrPtr, &clientLen); // Accetta una connessione dal client
    read(clientFd, &tmp, sizeof(tmp)); // Legge la somma dal client
    sum = ntohl(tmp);
    close(clientFd); // Chiude il client

    return sum;
}


int main()
{
    int sumP1, sumP2, sumP3;
    int tmp;
    int pidFailManager;
    int pidWatchDog;
    FILE *fpOutput, *fpSysLog;

    savePidOnFile("DF", getpid());
    createSocket();

    do
    {
        fpSysLog = fopen(SYSLOG, "w");
        if(fpSysLog == NULL)
        {
            printf("DF: error opening system_log\n");
            sleep(1);
        }
    }
    while(fpSysLog == NULL);

    do
    {
        fpOutput = fopen(OUTPUT, "w");
        if(fpOutput == NULL)
        {
            printf("DF: error opening voted_output\n");
            sleep(1);
        }
    }
    while(fpOutput == NULL);

    pidWatchDog = findPid("WD"); // Cerco il pid del watchDog per inviargli successivamente il segnale I_AM_ALIVE
    pidFailManager = findPid("FM"); // E quello del failureManager per inviargli SIGUSR1

    printf("DF:  READY\n");

    do
    {
        sumP1 = readProcess();
        sumP2 = readProcess();
        sumP3 = readProcess();

        if(sumP1 > 0 && sumP2 > 0 && sumP3 > 0) // Se viene ricevuta una somma negativa (quindi i processi hanno finito di leggere) salto la parte di scrittura su file
        {
            fprintf(fpOutput, "%d %d %d\n", sumP1, sumP2, sumP3); // Scrivo su file i risultati dei processi
            kill(pidWatchDog, SIGUSR2); // Manda I_AM_ALIVE al watchdog

            if(sumP1 == sumP2 || sumP1 == sumP3 || sumP2 == sumP3)   // Eseguo il voto di maggioranza
            {
                fprintf(fpSysLog, "SUCCESSO\n");
                printf("DF: SUCCESS\n");
            }
            else
            {
                fprintf(fpSysLog, "FALLIMENTO\n");
                printf("DF: FAILURE\n");
                // Vengono terminati i file in vista della terminazione del programma
                fclose(fpOutput);
                fclose(fpSysLog);
                kill(pidFailManager, SIGUSR1); // Manda un segnale SIGUSR1 a failureManager
            }
        }

    }
    while(sumP1 > 0 || sumP2 > 0 || sumP3 > 0);

    close(serverFd);
    fclose(fpSysLog);
    fclose(fpOutput);
    unlink(SOCKETDF);
    kill(pidFailManager, SIGUSR2); // Viene notificato il failManager che il programma ha terminato l'esecuzione e che anche lui può terminare

    return 0;
}
