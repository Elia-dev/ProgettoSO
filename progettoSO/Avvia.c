#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>

int main(int argc, char *argv[])
{
    int zero = 0;
    int uno = 1;
    FILE *fp = fopen("constHeader.h", "w"); // da dare percorso giusto

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
        fprintf(fp, "#define FILEPATH \"./fileP3\"\n#define PIDPATH \"filePid\"\n");
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
    system("rm inputManager & rm p1 & rm p2 & rm p3 & rm pipeP1 & rm decisionFunction & rm socketP2");
    system("rm socketDF & rm fileP3 & rm filePid & rm system_log & rm voted_output & rm failureManager");
    system("cc InputManager.c -o inputManager & cc P1.c -o p1 & cc P2.c -o p2 & cc P3.c -o p3");
    system("cc DecisionFunction.c -o decisionFunction & cc FailureManager.c -o failureManager");

    return 0;
}
