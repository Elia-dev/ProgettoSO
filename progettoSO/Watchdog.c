#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "ConstHeader.h"

// Genera il pid del processo e lo scrive in un file contenente tutti i processi
int generatePid()
{
    FILE *fp;
    do
    {
        fp = fopen(PIDPATH, "a");
        if(fp == NULL)
        {
            printf("WD: error opening file pid\n");
        }
    }
    while(fp == NULL);

    int pid = getpid();
    fprintf(fp, "WD: %d\n", pid); // Davanti al numero del pid vengono salvati due caratteri per identificare il processo
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
            printf("WD: error opening file pid\n");
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

void notifyFailureManager()
{
    printf("I_AM_ALIVE not received\n");
    int pidFailureManager = findPid("FM");
    kill(pidFailureManager, SIGUSR1);
}

void resetCounter()
{
    printf("I_AM_ALIVE received\n");
    alarm(2); // Reset del contatore
}

int main()
{
    // Quando viene ricevuto un segnale SIGUSR2 (I_AM_ALIVE) il contatore si resetta
    // Se il watchdog non riceve nessun I_AM_ALIVE entro due secondi viene attivato un allarme che verrà gestito
    // inviando un segnale SIGUSR1 al failureManager

    generatePid();
    signal(SIGUSR2, resetCounter);
    signal(SIGALRM, notifyFailureManager);
    alarm(2);
    printf("WD: READY\n");
    while(1)
    {
        usleep(100);
    }
    return 0;
}
