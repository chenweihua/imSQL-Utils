#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>

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

/*
    定义metadata的结构体，用于保存innobackupex的metadata数据。 
*/
typedef struct metadata{
    char *    metadata_type;
    long long metadata_from_lsn;
    long long metadata_to_lsn;
    long long metadata_last_lsn;
    int       xtrabackup_compact;
    char *    base_backup_directory;
    char *    backup_directory_name;
    char *    baseon_backup;
    char *    extra_lsndir;
}META;
    

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

typedef struct backup_file_name{
    /*
        备份文件名称一般为:
    first_name.hostname.backup_type,online_or_offline,compress_or_no,encrypt_or_no.timestamp.backup_number
        示例：
    all.hostname.0111.20151111154323.001
    parafiledb.rhel5.0010.20151111123212.001 
    */
    char *first_name;
    //备份开始名称，所有数据库用all，单个数据库就用这个数据库的名称.
    char *hostname;
    //备份程序运行在的机器的主机名。
    char *backup_type;
    //完整备份0，还是增量备份1.
    char *online_or_offline;
    //在线备份1,还是离线备份0.
    char *compress_or_no;
    //压缩用1表示，没压缩用0表示
    char *encrypt_or_no;
    //加密用1表示，没加密用0表示
    char *timestamp;
    //备份的时间戳
    char *backup_number;
    //当前时间备份的编号
}BFN;

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
    BFN *backup_file_name;
    char *hostname;
}INNOBAK;

int parse_database_conn_params(char *,DBP *);
int parse_innobackupex_params(char *,INNOBAK *);
int backup_database(PARA *,DBP *,INNOBAK *);
int restore_database(PARA *);
int operate_database_history(PARA *);
int pdb_shell(DBP *);
int connection_pdb_server(DBP *,MYSQL_RES *,MYSQL_ROW *,char *);
