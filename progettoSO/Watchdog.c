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
#include "ConstHeader.h"

// Genera il pid del processo e lo scrive in un file contenente tutti i processi
int generatePid()
{
    FILE *fp;
    do {
        fp = fopen(PIDPATH, "a");
        if(fp == NULL) {
            printf("WD: error opening file pid\n");
        }
    }while(fp == NULL);

    int pid = getpid();
    fprintf(fp, "WD: %d\n", pid); // Davanti al numero del pid vengono salvati due caratteri per identificare il processo
    fclose(fp);
    return pid;
}

int findPid(char *name) {
    char result[15];
    int pid;
    FILE *fp = fopen(PIDPATH, "r");
    printf("prima do while\n");
    do {
        fgets(result, 15, fp);
    }while(result[0] != name[0] || result[1] != name[1]); // Legge i vari pid finché non trova quello richiesto
    printf("dopo do while\n");
    char *substring;
    strtok(result, " ");
    substring = strtok(NULL, " ");
    pid = atoi(substring);
    fclose(fp);
    //printf("PID trovato: %d\n", pid);
    return pid;
}

void notifyFailureManager() {
    printf("NON E'ARRIVATO I AM ALIVE\n");
    int pidFailureManager = findPid("FM");
    kill(pidFailureManager, SIGUSR1);
}

void resetCounter() {
    printf("E' ARRIVATO I AM ALIVE\n");
    alarm(10);
}

int main() {

    generatePid();
    signal(SIGUSR2, resetCounter);
    signal(SIGALRM, notifyFailureManager);
    alarm(10);
    while(1) {

    }
    return 0;
}