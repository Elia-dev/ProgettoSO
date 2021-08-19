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
    do {
        fp = fopen(PIDPATH, "a");
        if(fp == NULL) {
            printf("FM: error opening file pid\n"); // Davanti al numero del pid vengono salvati due caratteri per identificare il processo
        }
    }while(fp == NULL);

    int pid = getpid();
    fprintf(fp, "FM: %d\n", pid);
    fclose(fp);
    return pid;
}

void endProgram() {
    printf("SIGUSR2 received, killing remaining processes [watchDog, failureManager]\n");
    system("killall watchdog & killall failureManager"); // Uccide gli ultimi processi rimasti in vita
}

void killAll() // Quando riceve il segnale termina tutti i processi
{
    printf("SIGUSR1 received, killing all processes\n");
    // Distruggo le socket e la pipe
    unlink(SOCKET);
    unlink(SOCKETDF);
    unlink(PIPE);

    system("killall p1 & killall p2 & killall p3 & killall decisionFunction & killall inputManager & killall watchdog & killall failureManager");
}

int main()
{
    // Alla ricezione di SIGUSR1 vengono terminati tutti i processi
    // Se viene ricevuto un SIGUSR2 vuol dire che il programma è terminato correttamente
    // endProgram() termina gli ultimi processi rimasti in vita

    generatePid();
    signal(SIGUSR1, killAll);
    signal(SIGUSR2, endProgram);
    printf("FM: READY\n");
    while(1) {

    }

    return 0;
}
