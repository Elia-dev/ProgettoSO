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
            printf("FM: error opening file pid\n"); // Davanti al numero del pid vengono salvati due caratteri per identificare il processo
        }
    }while(fp == NULL);

    int pid = getpid();
    fprintf(fp, "FM: %d\n", pid);
    fclose(fp);
    return pid;
}

void endProgram() {
    system("killall watchdog & killall failureManager");
}

void killAll()   // quando ricevo il segnale termino tutti i processi*/
{
    //signal(SIGUSR1, killAll);
    unlink(SOCKET); //Distruggo le socket e la pipe
    unlink(SOCKETDF);
    unlink(PIPE);

    system("killall p1 & killall p2 & killall p3 & killall decisionFunction & killall inputManager & killall watchdog & killall failureManager");
}

int main()
{
    signal(SIGUSR1, killAll);
    signal(SIGUSR2, endProgram);
    printf("FM: PRONTO");
    generatePid();
    while(1) {
        sleep(1);
    }
    //Riceve il segnale
    // Termina tutti i processi
    // FACOLTATIVO: riavvia P1, P2, P3
    return 0;
}
