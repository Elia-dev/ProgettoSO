#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define DEFAULT_PROTOCOL 0
#define PIPE "pipeP1"
#define SOCKET "socketP2"


int fd;
int serverFd;
int clientFd;

int openSocket()
{

    int serverLen, clientLen;
    struct sockaddr_un serverUNIXAddress; //Server address
    struct sockaddr* serverSockAddrPtr; //Ptr to server address
    struct sockaddr_un clientUNIXAddress; //Client address
    struct sockaddr* clientSockAddrPtr;//Ptr to client address

    serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
    serverLen = sizeof (serverUNIXAddress);
    clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
    clientLen = sizeof (clientUNIXAddress);

    serverFd = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    serverUNIXAddress.sun_family = AF_UNIX; //Set domain type
    strcpy (serverUNIXAddress.sun_path, SOCKET); // Set name
    unlink (SOCKET); // Remove file if it already exists
    bind (serverFd, serverSockAddrPtr, serverLen); //Create file
    listen (serverFd, 1); // Maximum pending connection length

    clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
    printf("In attessa di connessioni\n");
    //writeRecipe(clientFd); // Send the recipe
}

void sendToSocket(char *message)
{
    write (clientFd, message, strlen (message) + 1); /* Write first line */
}

int openPipe()
{
    int fd;

    do    //Keep trying to open the file until successful
    {
        fd = open (PIPE, O_WRONLY); //Open named pipe for writing
        if (fd == -1) sleep (1); // Try again in 1 second
    }
    while (fd == -1);

    return fd;
}


void sendToPipe(char *message)
{
    write (fd, message, strlen(message) + 1); //Write message down pipe
}

int main()
{
    char *path = "../dataset2.csv";
    char car;
    FILE *fp;
    int dimRiga = 0;

    fp = fopen(path, "r"); // Apertura del file in sola lettura
    printf("file aperto\n");
    //fd = openPipe();
    serverFd = openSocket();
    printf("SERVER PRONTO\n");

    // Scarto la prima riga leggendola a vuoto
    while(car != '\n')   // Legge la prima riga
    {
        car = fgetc(fp); // Leggendo carattere per carattere
        //printf("%c", car);
    }

    car = 'a'; // Reset del carattere di controllo
    //printf("\n");

    // Conta dimensioni righe
    while(car != '\n')
    {
        car = fgetc(fp);
        dimRiga++; // Conta di quanti caratteri è lunga la seconda riga, si assume che le righe abbiano la stessa lunghezza
    }

    // printf("dimRiga: %d\n", dimRiga);

    fseek(fp, -dimRiga, 1); // Mi riposiziono all'inizio della seconda riga
    char buffer[dimRiga];

    // fgets(buffer, dimRiga, fp);
    // strcat(buffer, "\0");
    //sendToPipe(buffer);
    // printf("%s\n\n\n", buffer);

    while(fgets(buffer, dimRiga, fp))  // Scorro tutto il file
    {
        printf("STRINGA LETTA DA FILE: %s\n\n", buffer);
        //sendToPipe(buffer);
        sendToSocket(buffer);

//printf("PRIMA DI SOCKET: %d\n", &buffer);

        // printf("DOPO SOCKET: %d\n", &buffer);
        /*  printf("STAMPA CON CICLO: ");
           for(int i = 0; i < dimRiga; i++) {
               printf("%c", buffer[i]);
           }*/
        //printf("\nStringa mandata tramite PIPE: %s", buffer);
        // printf("DOPO STAMPA: %d\n", &buffer);
        //  printf("\nStringa mandata tramite socket: %s", buffer);
        // printf("%s\n\n\n", buffer);
        sleep(1);
        // sendToSharedFile(buffer);
    }

    //close(fd); // Chiude il file descriptor della pipe
    fclose(fp); // Chiude il file dataset.csv
	close(clientFd); //Close the client
//printf("Client chiuso\n");
	close(serverFd); //Close the socket
	unlink(serverFd);
	printf("Server chiuso\n");
// printf("FINE");
    return 0;
}

