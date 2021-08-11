#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>




void createPipe(char *pathName) {
    mknod(pathName, O_WRONLY, 0);
    //pipe();
}

int main()
{
    char *path;
    path = "/home/filippo/Scrivania/progettoSO/dataset2.csv";
    char car;
    FILE *fp;
    printf("prova");

    int dimRiga = 0;

    fp = fopen(path, "r"); // Apertura del file in sola lettura

    // Scarto la prima riga leggendola a vuoto
    while(car != '\n') { // Legge la prima riga
        car = fgetc(fp); // Leggendo carattere per carattere
        printf("%c", car);
    }

    car = 'a';
    printf("\n");

    // Conta dimensioni righe
    while(car != '\n') {
        car = fgetc(fp);
        dimRiga++; // Conta di quanti caratteri è lunga la seconda riga, si assume che le righe abbiano la stessa lunghezza
    }

    printf("dimRiga: %d\n", dimRiga);

    fseek(fp, -dimRiga, 1); // Mi riposiziono all'inizio della seconda riga
    char buffer[dimRiga];


   while(!feof(fp)) { // Scorro tutto il file
        sleep(1);
        fgets(buffer, dimRiga, fp);
       // sendToPipe(buffer);
       // sendToSocket(buffer);
       // sendToSharedFile(buffer);
        printf("%s\n\n\n", buffer);
    }

    fclose(fp);
    return 0;
}

