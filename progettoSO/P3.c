#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h>

#define FILEPATH "./fileP3"
#define SOCKETDF "socketDecisionFunction"
#define DEFAULT_PROTOCOL 0

int sum(char *str)
{
/// da influenzare con la funzione random_failure
    int charSum = 0;
    for(int i = 0; i < strlen(str) - 1; i++) {   // -1 perchè l'ultimo carattere è '\n'
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
    // Se random failure è attivo e il numero generato tra 0 e 9 è uguale ad 1 allora genera una failure
    if(attivo && (rand()%10) == 1)
    {
        return 30;
    }
    else return 0;
}

int main()
{
    char str[700]; // 700 perchè le righe sono grosse circa 550 e sennò va fuori memoria e crasha
    int charSum = 0;
    //FILE *fp;
    // fp = fopen(FILEPATH, "r");


    int fd;
    fd = open(FILEPATH, O_RDONLY);
    printf("OPEN\n");

    while(readLine(fd, str) > 0)
    {
        printf("Stringa letta: %s\n", str);
        sleep(1);
        charSum = sum(str);
        charSum += random_failure(0);
        printf("Somma: %d: \n", charSum);
        //sendToDecisionFunction(charSum);
    }
    close (fd); // Close the socket
    printf("File chiuso\n");
    return 0;
}
