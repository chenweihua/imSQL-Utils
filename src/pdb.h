#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mysql/my_global.h>
#include <mysql/mysql.h>

extern char **environ;

#define DFTLENGTH 1024
char *pdb_conn_info = "/root/.conn";

typedef struct dbparams{
    char * host;
    char * user;
    char * pass;
    char * socket;
    int port;
}DBP;

typedef struct params{
    int pos;
    int length;
    char * content;
}PARA;

int parse_database_conn_params(char *,DBP *);
int backup_database(PARA *,DBP *);
int restore_database(PARA *);
int operate_database_history(PARA *);
int pdb_shell(DBP *);
int connection_pdb_server(DBP *,MYSQL_RES *,MYSQL_ROW *,char *);
