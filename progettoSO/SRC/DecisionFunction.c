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
FILE *fpOutput, *fpSysLog;

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

void openFile() { //FILE *fpOutput, FILE *fpSysLog
	do
    {
        fpSysLog = fopen(SYSLOG, "a");
        if(fpSysLog == NULL)
        {
            printf("DF: error opening system_log\n");
            sleep(1);
        }
    }
    while(fpSysLog == NULL);
    
    do
    {
        fpOutput = fopen(OUTPUT, "a");
        if(fpOutput == NULL)
        {
            printf("DF: error opening voted_output\n");
            sleep(1);
        }
    }
    while(fpOutput == NULL);
}

int main()
{
	savePidOnFile("DF", getpid());
    int sumP1, sumP2, sumP3;
    int tmp;
    int pidFailManager;
    int pidWatchDog;
    
    fpSysLog = fopen("./LOG/system_log", "w");
    fpOutput = fopen("./LOG/voted_output", "w");
	fclose(fpSysLog);
    fclose(fpOutput);
    
    createSocket();

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
        	openFile();
            fprintf(fpOutput, "%d %d %d\n", sumP1, sumP2, sumP3); // Scrivo su file i risultati dei processi
            fclose(fpOutput);
            kill(pidWatchDog, SIGUSR2); // Manda I_AM_ALIVE al watchdog

            if(sumP1 == sumP2 || sumP1 == sumP3 || sumP2 == sumP3)   // Eseguo il voto di maggioranza
            {
                fprintf(fpSysLog, "SUCCESSO\n");
                fclose(fpSysLog);
                printf("DF: SUCCESS\n");
            }
            else
            {
                fprintf(fpSysLog, "FALLIMENTO\n");
                fclose(fpSysLog);
                printf("DF: FAILURE\n");
                kill(pidFailManager, SIGUSR1); // Manda un segnale SIGUSR1 a failureManager
            }
        }

    }
    while(sumP1 > 0 || sumP2 > 0 || sumP3 > 0);

    close(serverFd);
    unlink(SOCKETDF);
    kill(pidFailManager, SIGUSR2); // Viene notificato il failManager che il programma ha terminato l'esecuzione e che anche lui può terminare

    return 0;
}
