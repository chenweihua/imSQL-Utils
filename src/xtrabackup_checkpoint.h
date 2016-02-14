#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "pdb.h"

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


/*
    定义metadata的结构体，用于保存innobackupex的metadata数据。 
*/
typedef struct metadata{
    char *    metadata_type;
    //metadata的状态类型.
    int       is_compressed;
    //innobackup备份文件的状态.
    long long metadata_from_lsn;
    //metadata的起始lsn号码
    long long metadata_to_lsn;
    //备份结束时，metadata的结束lsn号码
    long long metadata_last_lsn;
    //备份结束时，metadata的最新lsn号码
    int       xtrabackup_compact;
    //本次备份是否是compact备份，compact备份只包含主键，不包含二级索引。所以，此类型的备份相比于完整备份占用的磁盘空间更少.
    char *    base_backup_directory;
    //备份文件存放的基础路径
    char *    backup_directory_name;
    //备份文件的名称
    char *    baseon_backup;
    //完整备份的名称
    char *    extra_lsndir;
    //xtrabackup_checkpoints文件保存的路径.
}META;
 
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
    char *incremental_lsn;
}INNOBAK;

 
void xtrabackup_print_metadata(META*,char *,size_t);
int create_full_backup_directory_and_metadata(INNOBAK*,META *);
int read_xtrabackup_checkpoint_file(char *,META *);

