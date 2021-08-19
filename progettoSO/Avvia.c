#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include <wait.h>

int main(int argc, char *argv[])
{
    int zero = 0;
    int uno = 1;
    FILE *fp = fopen("constHeader.h", "w");

    //mkdir("LOG", 0777);  /*creo una cartella per i vari file di log*/
    //mkdir("BIN", 0777); /*creo una cartella per i file compilati*/
    if(argc>=3)  /*se sono stati passati meno di due argomenti termino con un errore critico*/
    {
        fprintf(fp, "#ifndef CONSTHEADER_H_INCLUDED\n#define CONSTHEADER_H_INCLUDED\n\n");
        //  char comp[1000];
        if(strcmp(argv[1], "NORMALE") == 0)
        {
            fprintf(fp, "#define MODEXEC %d\n", zero);
            // sprintf(comp, "./BIN/p3&./BIN/input %s&./BIN/dec&./BIN/p1&./BIN/p2&./BIN/fail&./BIN/watch", argv[2]);
        }
        else if(strcmp(argv[1], "FALLIMENTO") == 0)
        {
            fprintf(fp, "#define MODEXEC %d\n", uno);
            //sprintf(comp, "./BIN/p3 1&./BIN/input %s&./BIN/dec&./BIN/p1 1&./BIN/p2 1&./BIN/fail&./BIN/watch", argv[2]); /*inserisco il filepath da visitare nella stringa che eseguirò*/
        }
        else          /*Se la modalità di esecuzione passata non è valida termino con un errore critico*/
        {
            fclose(fp);
            system("rm constHeader.h");
            printf("mod di avvio non valida\n");
            // int fdrisultato=open("LOG/SYSTEM_LOG",O_WRONLY|O_CREAT|O_TRUNC,0777);
            //write(fdrisultato, "ERRORE CRITICO!",15);
            // close(fdrisultato);
            exit(-1);
        }

        fprintf(fp, "#define PATHDATASET \"%s\"\n", argv[2]);
        fprintf(fp, "#define FILEPATH \"fileP3\"\n#define PIDPATH \"filePid\"\n");
        fprintf(fp, "#define OUTPUT \"voted_output\"\n#define SYSLOG \"system_log\"\n");
        fprintf(fp, "#define DEFAULT_PROTOCOL %d\n#define SOCKET \"socketP2\"\n", zero);
        fprintf(fp, "#define SOCKETDF \"socketDF\"\n#define PIPE \"pipeP1\"\n\n");
        fprintf(fp, "#endif // CONSTHEADER_H_INCLUDED\n");
        fclose(fp);
        //fprintf("SOCKET \"sockeP1");
        /*   system("chmod 777 BIN");  /*imposto i permessi sulla directory BIN al massimo per poterla gestire senza problemi
           system("cc SRC/P3.c -o BIN/p3 &cc SRC/InputManager.c -o BIN/input&cc SRC/decision.c -o BIN/dec&cc SRC/P1.c -o BIN/p1&cc SRC/P2.c -o BIN/p2&cc SRC/Failure_Manager.c -o BIN/fail&cc SRC/Watchdog.c -o BIN/watch");
           mkdir("FILEPID", 0777);
           sleep(1);
           system("chmod 777 BIN/p2&chmod 777 BIN/p3&chmod 777 BIN/input&chmod 777 FILEPID&chmod 777 LOG"); imposto i permessi al massimo
           sleep(1);
           system(comp); */
    }
    else
    {
        fclose(fp);
        system("rm constHeader.h");
        printf("Inseriti num argomenti < 3\n");
        // int fdrisultato=open("LOG/SYSTEM_LOG",O_WRONLY|O_CREAT|O_TRUNC,0777);
        // write(fdrisultato, "ERRORE CRITICO!",15);
        // close(fdrisultato);
        exit(-1);
    }
/*
    system("rm inputManager & rm p1 & rm p2 & rm p3 & rm pipeP1 & rm decisionFunction & rm socketP2");
    system("rm socketDF & rm fileP3 & rm filePid & rm system_log & rm voted_output & rm failureManager & rm watchdog");
    system("cc InputManager.c -o inputManager & cc P1.c -o p1 & cc P2.c -o p2 & cc P3.c -o p3");
    system("cc DecisionFunction.c -o decisionFunction & cc FailureManager.c -o failureManager& cc Watchdog.c -o watchdog");

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
    printf("\n");*/
    return 0;
}
