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
/*
int readProcess(int *process) // per ora lasciamolo in caso si voglia rendere più leggibile sotto
{
    int tmp;
    read(clientFd, &process, sizeof(process));
    process = ntohl(process); // Converte da network byte order a int
    read(clientFd, &tmp, sizeof(tmp));
    return tmp = ntohl(tmp);
}
*/

// Genera il pid del processo e lo scrive in un file contenente tutti i processi
int generatePid()
{
    FILE *fp;
    do
    {
        fp = fopen(PIDPATH, "a");
        if(fp == NULL)
        {
            printf("DF: error opening file pid\n"); // Davanti al numero del pid vengono salvati due caratteri per identificare il processo
        }
    }
    while(fp == NULL);

    int pid = getpid();
    fprintf(fp, "DF: %d\n", pid);
    fclose(fp);
    return pid;
}

// Cerca all'interno del filePid il pid del nome del processo passato per argomento
int findPid(char *name)
{
    char result[15];
    int pid;
    FILE *fp;

    do
    {
        fp = fopen(PIDPATH, "r");
        if(fp == NULL)
        {
            printf("DF: error opening file pid\n");
        }
    }
    while(fp == NULL);

    do
    {
        fgets(result, 15, fp);
    }
    while(result[0] != name[0] || result[1] != name[1]);  // Legge i vari pid finché non trova quello richiesto

    char *substring;
    strtok(result, " ");
    substring = strtok(NULL, " "); // Viene presa solo la parte di stringa contenente il pid
    pid = atoi(substring); // E convertita da stringa ad int
    fclose(fp);
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
    usleep(500000);
    pidFailManager = findPid("FM"); // E quello del failureManager per inviargli SIGUSR1

    printf("DF:  READY\n");

    do
    {
        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen); // Accetta una connessione dal client
        read(clientFd, &tmp, sizeof(tmp)); // Legge il numero del processo dal client
        p1 = ntohl(tmp); // Converte da network byte order a int
        read(clientFd, &tmp, sizeof(tmp)); // Legge la somma dal client
        sommaP1 = ntohl(tmp);
        close(clientFd); // Chiude il client

        // Esegue la stessa operazione per gli altri due processi

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        read(clientFd, &tmp, sizeof(tmp));
        p2 = ntohl(tmp);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP2 = ntohl(tmp);
        close(clientFd);

        clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
        read(clientFd, &tmp, sizeof(tmp));
        p3 = ntohl(tmp);
        read(clientFd, &tmp, sizeof(tmp));
        sommaP3 = ntohl(tmp);
        close(clientFd);

        if(sommaP1 > 0 && sommaP2 > 0 && sommaP3 > 0) // Se viene ricevuta una somma negativa (quindi i processi hanno finito di leggere) salto la parte di scrittura su file
        {
            fprintf(fpOutput, "%d %d %d\n", sommaP1, sommaP2, sommaP3); // Scrivo su file i risultati dei processi
            kill(pidWatchDog, SIGUSR2); // Manda I_AM_ALIVE

            if(sommaP1 == sommaP2 || sommaP1 == sommaP3 || sommaP2 == sommaP3)   // Eseguo il voto di maggioranza
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
    while(sommaP1 > 0 || sommaP2 > 0 || sommaP3 > 0);

    close(serverFd);
    fclose(fpSysLog);
    fclose(fpOutput);
    unlink(SOCKETDF);
    kill(pidFailManager, SIGUSR2); // Viene notificato il failManager che il programma ha terminato l'esecuzione e che anche lui può terminare

    return 0;
}
