#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PIPE "P1"


int fd;

int openPipe() {
    int fd;

    do {  //Keep trying to open the file until successful
        fd = open (PIPE, O_WRONLY); //Open named pipe for writing
            if (fd == -1) sleep (1); // Try again in 1 second
    } while (fd == -1);

    return fd;
}


void sendToPipe(char *message) {
    write (fd, message, strlen(message) + 1); //Write message down pipe
}

int main()
{
    char *path = "../dataset2.csv";
    char car;
    FILE *fp;
    int dimRiga = 0;

    fp = fopen(path, "r"); // Apertura del file in sola lettura
    fd = openPipe();

    // Scarto la prima riga leggendola a vuoto
    while(car != '\n') { // Legge la prima riga
        car = fgetc(fp); // Leggendo carattere per carattere
        //printf("%c", car);
    }

    car = 'a'; // Reset del carattere di controllo
    //printf("\n");

    // Conta dimensioni righe
    while(car != '\n') {
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

   while(!feof(fp)) { // Scorro tutto il file
        fgets(buffer, dimRiga, fp);
        printf("STRINGA LETTA DA FILE: %s\n\n", buffer);
        sendToPipe(buffer);
        printf("%s\n\n\n", buffer);
        sleep(1);
       // sendToSocket(buffer);
       // sendToSharedFile(buffer);
    }

    close(fd);
    fclose(fp);
    printf("FINE");
    return 0;
}

