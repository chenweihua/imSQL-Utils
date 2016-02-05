#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>

extern char **environ;

//定义本程序内标准字符长度为1024.
#define DFTLENGTH 1024

/*
    定义两个配置文件
    /etc/sysconfig/pdb/db.properties用于定义数据库链接信息
    /etc/sysconfig/pdb/innobackupex用户innobackupex的参数信息
*/
char *pdb_conn_info = "/etc/sysconfig/pdb/db.properties";
char *inno_conn_info = "/etc/sysconfig/pdb/innobackupex";
char *hisdb = "/etc/sysconfig/pdb/hisdb";

  
/*
    定义连接到数据库用到的参数。 
*/
typedef struct dbparams{
    char * host;
    char * user;
    char * pass;
    char * socket;
    int port;
}DBP;

typedef struct params{
    int pos;
    int argclen;
    char * content;
}PARA;

int pdb_shell(DBP *);
static int list_backup_history_callbak(void *,int,char **,char **);
int list_backup_history(char *,PARA *);
int database_is_exists(PARA *);
void print_main_help(void);
void print_backup_help(void);
void print_restore_help(void);
void print_history_help(void);
void print_shell_help(void);
