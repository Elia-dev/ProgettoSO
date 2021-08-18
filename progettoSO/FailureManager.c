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
    unlink(SOCKET); //Distruggo le socket e la pipe
    unlink(SOCKETDF);
    unlink(PIPE);
    system("killall p1 & killall p2 & killall p3 & killall decisionFunction & killall inputManager & killall failureManager"); // & killall watch
}

int main()
{
    signal(SIGUSR1, killAll);
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
