#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mysql/my_global.h>
#include <mysql/mysql.h>

extern char **environ;

#define DFTLENGTH 1024

/*
    定义两个配置文件
    .conn用于定义数据库链接信息
    .inno用户innobackupex的参数信息
*/
char *pdb_conn_info = "/root/.conn";
char *inno_conn_info = "/root/.inno";

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

typedef struct innobak{
    char *extra_lsndir;
    char *innobak_bin;  //innobackupex执行文件路径
    int parallel;       //并行进程数
    int throttle;
    char *encrypt;      //压缩文件加密算法
    char *encrypt_key_file; //压缩算法密钥文件
    int compress;
    int compress_threads;
    char *stream;
    char *use_memory;   //innobackupex备份时使用内存
    char *todir;        //备份文件存储路径,用于备份操作
    char *fromdir;      //备份文件存储路径,用于恢复操作
    char *intodir;      //备份文件恢复路径,用于恢复操作
    char *backup_file_name;
}INNOBAK;

int parse_database_conn_params(char *,DBP *);
int parse_innobackupex_params(char *,INNOBAK *);
int backup_database(PARA *,DBP *,INNOBAK *);
int restore_database(PARA *);
int operate_database_history(PARA *);
int pdb_shell(DBP *);
int connection_pdb_server(DBP *,MYSQL_RES *,MYSQL_ROW *,char *);
