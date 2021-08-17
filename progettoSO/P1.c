#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h> // For AF_UNIX sockets

#define PIPE "pipeP1"
#define DEFAULT_PROTOCOL 0
#define SOCKETDF "socketDecisionFunction"

int clientFd;

int openSocket()
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
            printf("Retrying connection in 1 sec\n");
            sleep (1); // Wait and then try again
        }
    }
    while (connection == -1);

    return clientFd;
}

int readLine (int fd, char *str)
{
    // Read a single '\0'-terminated line into str from fd /
    // Return 0 when the end-of-input is reached and 1 otherwise /
    int n;
    do
    {
        // Read characters until '\0' or end-of-input /
        n = read(fd, str, 1);
    }
    while (n > 0 && *str++ != '\0');
    return (n > 0);
    // Return false if end-of-input /
}

void createPipe()
{
    // Remove named pipe if it already exists /
    unlink(PIPE);
    // Create named pipe /
    mknod (PIPE, S_IFIFO, 0);
    // Change its permissions /
    chmod (PIPE, 0660);
}

int sum(char *token)
{
/// da influenzare con la funzione random_failure
    int charSum = 0;
    for(int i = 0; i < strlen(token); i++)
    {
        charSum += token[i]; // Somma tutti i caratteri provenienti dal token
    }
    //printf("Somma parziali: [%d]\n", charSum);
    return charSum;
}

int random_failure(int attivo)
{
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale ad 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 10;
    }
    else return 0;
}

void sendToDecisionFunction(int socket, int sum) {
    int array[2] = {1, sum};
    write (clientFd, array, 2); /* Write first line */
}

int main()
{
    int fd;
    char str[700]; // 700 perchè le righe sono grosse circa 550 e sennò va fuori memoria e crasha
    char *token;
    int charSum = 0;
    createPipe();
    int socket = openSocket();
    fd = open (PIPE, O_RDONLY); //O_RDONLY
    printf("PIPE APERTA");
    while(readLine (fd, str) > 0)
    {
        charSum = 0;
        printf("Letto: %s\n\n\n", str);
        str[strlen(str) - 1] = '\0'; // Alla fine di ogni riga viene sovrascritto il carattere "end of trans. block" (valore ascii 23) con '\0'
        token = strtok(str, ",");

        // Tramite strtok la riga viene spezzata ogni volta che si trova una virgola e il suo valore intero sommato
        while( token != NULL )
        {
            charSum += sum(token);
            token = strtok(NULL, ",");
        }
        charSum += random_failure(0);
        printf("somma: %d \n", charSum);
        sendToDecisionFunction(socket, charSum);
    }
    close(socket);
    close (fd); //Close pipe
    unlink(PIPE); //Remove used pipe
    return 0;
}
