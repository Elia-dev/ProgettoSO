#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For write(), sleep(), read()
#include <signal.h>
#include <fcntl.h> // For open() constants
#include <sys/un.h> // For AF_UNIX sockets
#include <arpa/inet.h> // For order byte network
#include <time.h>
#include "constHeader.h"

int clientFd;


void openSocket()
{
    int serverLen, connection;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr* serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; // Server domain
    strcpy (serverUNIXAddress.sun_path, SOCKETDF);// Server name
    do   // Loop until a connection is made with the server
    {
        connection = connect (clientFd, serverSockAddrPtr, serverLen);
        if (connection == -1) // Se la connessione fallisce
        {
            printf("P3: Retrying connection in 1 sec\n");
            sleep (1); // Wait and then try again
        }
    }
    while (connection == -1);
}

int sum(char *str)
{
/// da influenzare con la funzione random_failure
    int charSum = 0;
    for(int i = 0; i < strlen(str) - 1; i++)     // -1 perchè l'ultimo carattere è '\n'
    {
        charSum += str[i] * (str[i] != 44); // 44 = virgola
        //printf("INT: %d CHAR: %c", str[i], str[i]);
    }
    //printf("\n");
    return charSum;
}

int readLine (int fd, char *str)
{
    int n;
    do // Read characters until '\0' or end-of-input
    {
        n = read(fd, str, 1);
    }
    while (n > 0 && *str++ != '\0');
    return (n > 0); // Return false if end-of-input
}

/*
int readLine (FILE *fp, char *str)
{
    char car;
    int i = 0;
    do
    {
        car = fgetc(fp); // Leggendo carattere per carattere
        str[i++] = car;
    }
    while(car > 0 && car != '\n');
    return (car != EOF);
}
*/

int random_failure(int attivo)
{
    time_t t;
    srand((unsigned)time(&t));
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale ad 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 30;
    }
    else return 0;
}

void sendToDecisionFunction(int sum)
{
    int tmp;
    if(sum >= 0)
    {
        tmp = htonl(3); // Converte sum in network Byte Order
        write(clientFd, &tmp, sizeof(tmp));
        tmp = htonl(sum); // Converte sum in network Byte Order
        write(clientFd, &tmp, sizeof(tmp));
    }
    else
    {
        tmp = htonl(3); // Converte sum in network Byte Order
        write(clientFd, &tmp, sizeof(tmp));
        tmp = htonl(-1); // Converte sum in network Byte Order
        write(clientFd, &tmp, sizeof(tmp));
    }

    //write (clientFd, str2, strlen(str2) + 1); /* Write first line */
}

int generatePid()   //metodo che genera un file contenente il PID di questo processo
{
    FILE *fp = fopen(PIDPATH, "a");
    int pid = getpid();
    fprintf(fp, "P3: %d\n", pid);
    fclose(fp);
    return pid;
}

int main()
{
    char str[700]; // 700 perchè le righe sono grosse circa 550 e sennò va fuori memoria e crasha
    int charSum = 0;
    int fd;
    do
    {
        fd = open(FILEPATH, O_RDONLY);
        if(fd < 0) {
            printf("P3: Errore durante l'apertura del file...\n");
            sleep(1);
        }
    }
    while(fd < 0);

    generatePid();
    printf("P3: PRONTO\n");
    while(readLine(fd, str) > 0)
    {
        //printf("Stringa letta: %s\n", str);
        charSum = sum(str);
        charSum += random_failure(MODEXEC);
        openSocket();
        printf("P3: SOCKET APERTA\n");
        sendToDecisionFunction(charSum);
        close(clientFd);
        printf("P3: SOCKET CHIUSA\n");
        sleep(1);
    }
    openSocket();
    sendToDecisionFunction(-1);
    close(clientFd);
    close (fd);
    printf("P3: TERMINATO\n");
    return 0;
}
