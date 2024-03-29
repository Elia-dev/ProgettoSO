/******************************************************************
	Programma scritto da:
	Filippo Zaccari 7030220 filippo.zaccari@stud.unifi.it
	Elia Matteini 7033383 elia.matteini@stud.unifi.it
	Ilaria Catone 7020875 ilaria.catone@stud.unifi.it
	Data di consegna: 23/08/2021
******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <wait.h>

int main(int argc, char *argv[])
{
    /*
        All'interno del file avvia viene creato il file header utilizzato da tutti i processi
    */
    int zero = 0;
    int uno = 1;
    FILE *fpDataset;
    int c = 0;
    do { 
    	fpDataset = fopen(argv[2], "r"); // Provo a vedere se il file passato esiste davvero, se non esiste o non riesce ad aprirlo termino il programma
    	if(fpDataset == NULL) {
    		printf("MAIN PROCESS: ERROR OPENING DATASET\n");
    		c++;
    		sleep(1);
    	}
    	else {
    		break;
    	}
    	if(c == 3) exit(-1);
    }while(c < 3);
    fclose(fpDataset);
    
    FILE *fp = fopen("./SRC/ConstHeader.h", "w"); // Creazione file header
    FILE *fpPid  = fopen("filePid", "w"); // Creazione del file Pid
    fclose(fpPid);
	usleep(100);
    if(argc == 3) { // Se sono stati passati meno di due argomenti termino
        fprintf(fp, "#ifndef CONSTHEADER_H_INCLUDED\n#define CONSTHEADER_H_INCLUDED\n\n");

        if(strcmp(argv[1], "NORMALE") == 0) {
            fprintf(fp, "#define MODEXEC %d\n", zero);
        }
        else if(strcmp(argv[1], "FALLIMENTO") == 0) {
            fprintf(fp, "#define MODEXEC %d\n", uno);

        }
        else { //Se la modalità di esecuzione passata non è valida termino
            fclose(fp);
            printf("Invalid execution mode\n");
            exit(-1);
        }

        fprintf(fp, "#define PATHDATASET \"%s\"\n", argv[2]);
        fprintf(fp, "#define FILEPATH \"./LOG/fileP3\"\n#define PIDPATH \"./LOG/filePid\"\n");
        fprintf(fp, "#define OUTPUT \"./LOG/voted_output\"\n#define SYSLOG \"./LOG/system_log\"\n");
        fprintf(fp, "#define DEFAULT_PROTOCOL %d\n#define SOCKET \"./LOG/socketP2\"\n", zero);
        fprintf(fp, "#define SOCKETDF \"./LOG/socketDF\"\n#define PIPE \"./LOG/pipeP1\"\n");
        fprintf(fp, "#define I_AM_ALIVE SIGUSR2\n\n");
        fprintf(fp, "int savePidOnFile(char *name, int pid);\n");
        fprintf(fp, "int findPid(char *name);\n\n");
        fprintf(fp, "#endif // CONSTHEADER_H_INCLUDED\n");
        fclose(fp);
    }
    else {
        fclose(fp);
        printf("Invalid number of arguments\n");
        exit(-1);
    }
    
    system("make clean");
    system("make");
    usleep(50000);
    system("make install");
	usleep(50000);

	if (fork() == 0) {
        printf("--watchDog Avviato\n");
        execl("./BIN/watchdog", "./BIN/watchdog", (char *) NULL);
    }
    usleep(80000);
    if (fork() == 0) {
        printf("--failureManager Avviato\n");
        execl("./BIN/failureManager", "./BIN/failureManager", (char *) NULL);

    }
    usleep(70000);
    if (fork() == 0) {
        printf("--inputManager Avviato\n");
        execl("./BIN/inputManager", "./BIN/inputManager", (char *) NULL);
    }
    usleep(60000);
    if (fork() == 0) {
        printf("--p1 Avviato\n");
        execl("./BIN/p1", "./BIN/p1", (char *) NULL);
    }
    usleep(50000);
    if (fork() == 0) {
        printf("--p2 Avviato\n");
        execl("./BIN/p2", "./BIN/p2", (char *) NULL);

    }
    usleep(50000);
    if (fork() == 0) {
        printf("--p3 Avviato\n");
        execl("./BIN/p3", "./BIN/p3", (char *) NULL);
    }
    usleep(30000);
    if (fork() == 0) {
        printf("--decisionFunction Avviato\n");
        execl("./BIN/decisionFunction", "./BIN/decisionFunction", (char *) NULL);
    }

    for (int i = 0; i < 7; i++) {
        int z = wait(NULL);
        printf("%d: STOPPED\n", z);
    }

    return 0;
}
