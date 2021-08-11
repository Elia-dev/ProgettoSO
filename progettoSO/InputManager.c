#include <stdio.h>
#include <stdlib.h>

void readFile(char *path) {
    char *string;
    FILE *fp;
    fp = fopen(path, "r"); // Apertura del file in sola lettura

    fgets(string, 2048, fp); // Leggo a vuoto la prima riga
    printf(string);


    fclose(fp);
}
