#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "ConstHeader.h"

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
	printf("WD: READY\n");
    savePidOnFile("WD", getpid());
    printf("WD: DOPO pid\n");
    signal(I_AM_ALIVE, resetCounter);
    signal(SIGALRM, notifyFailureManager);
    alarm(2);
    
    while(1) {
        usleep(100);
    }
    return 0;
}
