#ifndef CONSTHEADER_H_INCLUDED
#define CONSTHEADER_H_INCLUDED

#define MODEXEC 1
#define PATHDATASET "../dataset2.csv"
#define FILEPATH "./../LOG/fileP3"
#define PIDPATH "./../LOG/filePid"
#define OUTPUT "./../LOG/voted_output"
#define SYSLOG "./../LOG/system_log"
#define DEFAULT_PROTOCOL 0
#define SOCKET "./../LOG/socketP2"
#define SOCKETDF "./../LOG/socketDF"
#define PIPE "./../LOG/pipeP1"

int savePidOnFile(char *name, int pid);
int findPid(char *name);

#endif // CONSTHEADER_H_INCLUDED
