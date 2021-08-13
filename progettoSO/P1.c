#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PIPE "pipeP1"

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

int main()
{
    int fd;
    char str[700]; // 700 perchè le righe sono grosse circa 550 e sennò va fuori memoria e crasha
    char *token;
    int charSum = 0;

    createPipe();

    fd = open (PIPE, O_RDONLY); //O_RDONLY
    printf("PIPE APERTA");
    int count = 0;
    while(count < 20)
    {
        readLine (fd, str); // Leggiamo riga per riga
        printf("Letto: %s\n\n\n", str);
        str[strlen(str) - 1] = '\0'; // Alla fine di ogni riga viene sovrascritto il carattere "end of trans. block" (valore ascii 23) con '\0'
        token = strtok(str, ",");
        printf("smongolato con end of trans block");
        // Tramite strtok la riga viene spezzata ogni volta che si trova una virgola e il suo valore intero sommato
        while( token != NULL )
        {
            //printf("-----%s\n", token);
            charSum += sum(token);
            //printf("%d\n", charSum);
            token = strtok(NULL, ",");
        }
        printf("Somma riga %d = %d", count, charSum);
        count++;
        //sendToDecisionFunction(charSum);

        //printf("somma: %d", charSum);
        /* do {
             token = strtok(str, ",");
             printf("%s", token);
         }while(token != NULL);*/

        //printf ("%s\n", str);
    }
    close (fd);
    // Close pipe /
    unlink(PIPE);
    // Remove used pipe */

    return 0;
}
