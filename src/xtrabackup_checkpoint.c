#include "xtrabackup_checkpoint.h"

/***********************************************************************
 *把metadata数据保存到buf缓冲区内.
 *本函数不设返回值
 *作者: Tian, Lei [tianlei@paratera.com]
 *时间: 20151019PM1318
*/
void
xtrabackup_print_metadata(META * metadata,char *buf, size_t buf_len)
{
    /* Use UINT64PF instead of LSN_PF here, as we have to maintain the file
     *     format. */
    snprintf(buf, buf_len,
            "backup_type = %s\n"
            "from_lsn = %lld\n" 
            "to_lsn = %lld\n" 
            "last_lsn = %lld\n" 
            "base_backup_directory = %s\n"
            "backup_directory_name = %s\n"
            "baseon_backup = %s\n"
            "compact = %d\n",
            metadata->metadata_type,
            metadata->metadata_from_lsn,
            metadata->metadata_to_lsn,
            metadata->metadata_last_lsn,
            metadata->base_backup_directory,
            metadata->backup_directory_name,
            metadata->baseon_backup,
            metadata->xtrabackup_compact ==0);
}



/***********************************************************************
 *innobackup命令做增量备份的时候会参考最后一次完整备份的xtrabackup_checkpoints文件.本函数就是在指定的目录extra_lsndir中创建这个文件。并且把metadata中的数据写入到这个文件。
 *成功返回0，否则返回1
 *作者: Tian, Lei [tianlei@paratera.com]
 *时间: 20151019PM1318
*/
int
create_full_backup_directory_and_metadata(INNOBAK *innobak,META *metadata)
{

    int len = 0;
    FILE *fp = NULL;

    //定义完整备份的checkpoints文件及保存路径。
    static char *full_backup_directory = NULL;
    static char *full_backup_metadata_file = NULL;
    static char *buf = NULL;
    full_backup_directory = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(full_backup_directory,0,DFTLENGTH/2);
    full_backup_metadata_file= (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(full_backup_metadata_file,0,DFTLENGTH/2);
    buf = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(buf,0,DFTLENGTH*2);

    snprintf(full_backup_metadata_file,511,"%s/xtrabackup_checkpoints",innobak->extra_lsndir);

    umask(S_IWGRP | S_IWOTH);
    mkdir(full_backup_directory,0777);

    //把metadata数据写入到xtrabackup_checkpoints文件中。
    xtrabackup_print_metadata(metadata,buf,2047);

    len = strlen(buf);

    //打开一个文件用于保存metadata数据。
    fp = fopen(full_backup_metadata_file, "w");
    if(!fp) {
        perror("fopen()");
        return(1);
    }
    //写入metadata信息
    if (fwrite(buf, len, 1, fp) < 1) {
        fclose(fp);
        return(1);
    }

    //释放分配的资源
    free(full_backup_directory);
    free(full_backup_metadata_file);
    free(buf);

    fclose(fp);

    return(0);
}


/**********************************************************************
 *从xtrabackup命令备份生成的metadata文件中读取meta数据，然后把这些数据保存到META结构体中.
 *返回值：正常返回0，否则返回非零
 *作者: Tian, Lei [tianlei@paratera.com]
 *时间: 20151019PM1713
 *********************************************************************/
int
read_xtrabackup_checkpoint_file(char *extra_lsndir,META *metadata)
{
    //创建一个文件描述符用于读取xtrabackup命令生成的xtrabackup_checkpoints文件 
    FILE *fp = NULL;
    int  r = 0;
    int  t;
    
    //创建局部变量用于保存xtrabackup_checkpoints文件的绝对路径
    static char *chk = NULL;
    chk = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(chk,0,DFTLENGTH/2);

    //读取xtrabackup_checkpoints文件.
    snprintf(chk,DFTLENGTH/2-1,"%s/xtrabackup_checkpoints",extra_lsndir);
    if(strlen(chk) <= 0){
        return(1);
    }

    fp = fopen(chk,"r");
    if(fp == NULL){
        perror("fopen()");
        return(2);
    }

    if (fscanf(fp, "backup_type = %s\n", metadata->metadata_type)
                    != 1) {
            r = 3;
            goto end;
    }
    if (fscanf(fp, "from_lsn = %lld\n", &metadata->metadata_from_lsn)
            != 1) {
        r = 4;
        goto end;
    }
    if (fscanf(fp, "to_lsn = %lld\n", &metadata->metadata_to_lsn)
            != 1) {
        r = 5;
        goto end;
    }
    if (fscanf(fp, "last_lsn = %lld\n", &metadata->metadata_last_lsn)
            != 1) {
        metadata->metadata_last_lsn = 0;
    }
    /*compact表示只备份主键索引，不备份二级索引 */
    if (fscanf(fp, "compact = %d\n", &t) == 1) {
        metadata->xtrabackup_compact = (t == 1);
    } else {
        metadata->xtrabackup_compact  = 0;
    }

    snprintf(metadata->extra_lsndir,DFTLENGTH/2-1,"%s",extra_lsndir);

//错误处理
end:
    fclose(fp);
    free(chk);

    return(r);
}


