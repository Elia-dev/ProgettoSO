#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ConstHeader.h"

// Genera il pid del processo e lo scrive in un file contenente tutti i processi
int savePidOnFile(char *name, int pid) 
{
    FILE *fp;
    do {
        fp = fopen(PIDPATH, "a");
        if(fp == NULL) {
            printf("%s: error opening file pid\n", name); // Davanti al numero del pid vengono salvati due caratteri per identificare il processo
        }
    }while(fp == NULL);

    fprintf(fp, "%s: %d\n", name, pid);
    fclose(fp);
    return pid;
}

// Cerca all'interno del filePid il pid del nome del processo passato per argomento
int findPid(char *name)
{
    char result[15];
    int pid;
    FILE *fp;

    do
    {
        fp = fopen(PIDPATH, "r");
        if(fp == NULL) {
            printf("%s: error opening file pid\n", name);
        }
    }
    while(fp == NULL);

    do {
        fgets(result, 15, fp);
    }
    while(result[0] != name[0] || result[1] != name[1]);  // Legge i vari pid finché non trova quello richiesto

    char *substring;
    strtok(result, " ");
    substring = strtok(NULL, " "); // Viene presa solo la parte di stringa contenente il pid
    pid = atoi(substring); // E convertita da stringa ad int
    fclose(fp);
    return pid;
}
