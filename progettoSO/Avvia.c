#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include <wait.h>

int main(int argc, char *argv[])
{
/*
    All'interno del file avvia viene creato il file header utilizzato da tutti i processi
*/
    int zero = 0;
    int uno = 1;
    FILE *fp = fopen("./SRC/ConstHeader.h", "w"); // Creazione file header
    FILE *fpPid  = fopen("filePid", "w"); // Creazione del file Pid
    fclose(fpPid);

    if(argc == 3)  // Se sono stati passati meno di due argomenti termino
    {
        fprintf(fp, "#ifndef CONSTHEADER_H_INCLUDED\n#define CONSTHEADER_H_INCLUDED\n\n");

        if(strcmp(argv[1], "NORMALE") == 0)
        {
            fprintf(fp, "#define MODEXEC %d\n", zero);
        }
        else if(strcmp(argv[1], "FALLIMENTO") == 0)
        {
            fprintf(fp, "#define MODEXEC %d\n", uno);

        }
        else //Se la modalità di esecuzione passata non è valida termino
        {
            fclose(fp);
            system("rm constHeader.h");
            printf("Invalid execution mode\n");
            exit(-1);
        }

        fprintf(fp, "#define PATHDATASET \"%s\"\n", argv[2]); // cambiare percorso file da mettere in LOG
        fprintf(fp, "#define FILEPATH \"./../LOG/fileP3\"\n#define PIDPATH \"./../LOG/filePid\"\n");
        fprintf(fp, "#define OUTPUT \"./../LOG/voted_output\"\n#define SYSLOG \"./../LOG/system_log\"\n");
        fprintf(fp, "#define DEFAULT_PROTOCOL %d\n#define SOCKET \"./../LOG/socketP2\"\n", zero);
        fprintf(fp, "#define SOCKETDF \"./../LOG/socketDF\"\n#define PIPE \"./../LOG/pipeP1\"\n\n");
        fprintf(fp, "int savePidOnFile(char *name, int pid);\n");
        fprintf(fp, "int findPid(char *name);\n\n");
        fprintf(fp, "#endif // CONSTHEADER_H_INCLUDED\n");
        fclose(fp);
    }
    else
    {
        fclose(fp);
        system("rm constHeader.h");
        printf("Invalid number of arguments\n");
        exit(-1);
    }
   // system("rm pipeP1 & rm socketP2");
   // system("rm socketDF & rm fileP3 & rm filePid & rm system_log & rm voted_output");
    system("make clean");
    system("make");
    system("make install");

    /*
    provare serie di fork ed exec
    */
    /*
    if (fork() == 0)
    {
        printf("--watchDog Avviato\n");
        execl("watchDog", "watchDog", (char *) NULL);
    }
    usleep(800000);
    if (fork() == 0)
    {
        printf("--failureManager Avviato\n");
        execl("failureManager", "failureManager", (char *) NULL);
    }
    usleep(500000);
    if (fork() == 0)
    {
        printf("--inputManager Avviato\n");
        execl("inputManager", "inputManager", (char *) NULL);
    }
    usleep(400000);
    if (fork() == 0)
    {
        printf("--p1 Avviato\n");
        execl("p1", "p1", (char *) NULL);
    }
    usleep(300000);
    if (fork() == 0)
    {
        printf("--p2 Avviato\n");
        execl("p2", "p2", (char *) NULL);
    }
    usleep(200000);
    if (fork() == 0)
    {
        printf("--p3 Avviato\n");
        execl("p3", "p3", (char *) NULL);
    }
    usleep(100000);
    if (fork() == 0)
    {
        printf("--decisionFunction Avviato\n");
        execl("decisionFunction", "decisionFunction", (char *) NULL);
    }
    /*
            int i = 0;
            while(i < 6)
            {
                printf("VALORE DELLA I: %d\n", i);
                if (fork() == 0)
                {
                    if (i == 0)
                    {
                        sleep(3);
                        execl("inputManager", "inputManager", (char *) NULL);
                        //execvp("./inputManager", argv);
                    }
                    else if (i == 1)
                    {
                        sleep(3);
                        execl("p1", "p1", (char *) NULL);
                        //execvp("./decisionFunction", argv);
                    }
                    else if (i == 2)
                    {
                        sleep(3);
                        execl("p2", "p2", (char *) NULL);
                        //execvp("./failureManager", argv);
                    }
                    else if(i == 3)
                    {
                        sleep(3);
                        execl("p3", "p3", (char *) NULL);
                        //execvp("./p1", argv);
                    }
                    else if (i == 4)
                    {
                        sleep(3);
                        execl("decisionFunction", "decisionFunction", (char *) NULL);
                        //execvp("./p2", argv);
                    }
                    else if (i == 5)
                    {
                        sleep(3);
                        execl("failureManager", "failureManager", (char *) NULL);
                        //execvp("./p3", argv);
                    }
                }
                i++;
                usleep(200);
            }
        */
/*
    for (int i = 0; i < 7; i++)
    {
        int z = wait(NULL);
        printf("\n%d Has crashed stopped", z);
    }
    */
    return 0;
}
