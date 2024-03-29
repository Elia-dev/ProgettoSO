#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "ConstHeader.h"

void endProgram() 
{
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

    savePidOnFile("FM", getpid());
    signal(SIGUSR1, killAll);
    signal(SIGUSR2, endProgram);
    printf("FM: READY\n");
    while(1) {
        usleep(100);
    }

    return 0;
}
