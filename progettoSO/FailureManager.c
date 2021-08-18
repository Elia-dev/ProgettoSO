#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // For open() constants
#include <unistd.h> // For write(), sleep(), read()...
#include <signal.h>
#include "constHeader.h"


int generatePid()   //metodo che genera un file contenente il PID di questo processo
{
    FILE *fp = fopen(PIDPATH, "a");
    int pid = getpid();
    fprintf(fp, "FM: %d\n", pid);
    fclose(fp);
}

void killAll()   // quando ricevo il segnale termino tutti i processi*/
{
    //signal(SIGUSR1, killAll);
    printf("non sono stato eseguito :(");
    unlink(SOCKET); //Distruggo le socket e la pipe
    unlink(SOCKETDF);
    unlink(PIPE);
    system("killall p1 & killall p2 & killall p3 & killall decisionFunction & killall inputManager & killall failureManager"); // & killall watch
    printf("non sono stato eseguito :(");
}

int main()
{
    generatePid();
    signal(SIGUSR1, killAll);
    printf("sus");
    while(1) {
        sleep(1);
    }
    printf("sus");
    //Riceve il segnale
    // Termina tutti i processi
    // FACOLTATIVO: riavvia P1, P2, P3
    return 0;
}
