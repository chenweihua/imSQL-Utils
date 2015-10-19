#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>

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
    

/**********************************************************************
 *Read xtrabackup_checkpoints in xtrabackup directory.
 *Save metadata informations to metadata struct.
 *@Return TRUE on success.FALSE on failure.
 *Author: Tian, Lei [tianlei@paratera.com]
 *Date: 20151019PM1713
 *********************************************************************/
static my_bool
read_xtrabackup_checkpoint_file(char *base_backup_directory,char *backup_directory_name,META *metadata)
{
    FILE *fp = NULL;
    my_bool  r = TRUE;
    int  t;


    static char *chk = NULL;
    chk = (char *)malloc(sizeof(char)*512);
    memset(chk,0,512);

    

    //read backup_directory_name files.
    snprintf(chk,511,"%s/%s/xtrabackup_checkpoints",base_backup_directory,backup_directory_name);
    if(strlen(chk) <= 0){
        return(FALSE);
    }

    fp = fopen(chk,"r");
    if(fp == NULL){
        perror("fopen()");
        return(FALSE);
    }

    if (fscanf(fp, "backup_type = %s\n", metadata->metadata_type)
                    != 1) {
            r = FALSE;
            goto end;
    }
    if (fscanf(fp, "from_lsn = %lld\n", &metadata->metadata_from_lsn)
            != 1) {
        r = FALSE;
        goto end;
    }
    if (fscanf(fp, "to_lsn = %lld\n", &metadata->metadata_to_lsn)
            != 1) {
        r = FALSE;
        goto end;
    }
    if (fscanf(fp, "last_lsn = %lld\n", &metadata->metadata_last_lsn)
            != 1) {
        metadata->metadata_last_lsn = 0;
    }
    /* Optional field */
    if (fscanf(fp, "compact = %d\n", &t) == 1) {
        metadata->xtrabackup_compact = (t == 1);
    } else {
        metadata->xtrabackup_compact  = 0;
    }
    if (fscanf(fp, "base_backup_directory = %s\n", metadata->base_backup_directory)
                    != 1) {
            r = FALSE;
            goto end;
    }
    if (fscanf(fp, "backup_directory_name = %s\n", metadata->backup_directory_name)
                    != 1) {
            r = FALSE;
            goto end;
    }
    if (fscanf(fp, "baseon_backup = %s\n", metadata->baseon_backup)
                    != 1) {
            r = FALSE;
            goto end;
    }
    if (fscanf(fp, "extra_lsndir = %s\n", metadata->extra_lsndir)
                    != 1) {
            r = FALSE;
            goto end;
    }

end:
    fclose(fp);
    free(chk);

    return(r);
}

/***********************************************************************
 *Write backup meata info into MySQL Database.
 *@return TRUE on success,FALSE on failure.
 *Author: Tian, Lei [tianlei@paratera.com]
 *Date:20151019PM1318
 ************************************************************************/
static my_bool
xtrabackup_write_metadata_into_db(META *metadata)
{
    //保存查询语句的缓冲区
    char *buf = NULL;
    buf = (char *)malloc(sizeof(char)*2048);
    memset(buf,0,2048);

    snprintf(buf,2047,"INSERT INTO sysadmin.t_xtra_backup_metadata(metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir) VALUES('%s',%lld,%lld,%lld,%lld,'%s','%s','%s','%s')",metadata->metadata_type,(long long)metadata->metadata_from_lsn,(long long)metadata->metadata_to_lsn,(long long)metadata->metadata_last_lsn,(int)metadata->xtrabackup_compact,metadata->base_backup_directory,metadata->backup_directory_name,metadata->baseon_backup,metadata->extra_lsndir);

    //创建数据库连接参数
    static char *ipaddr = NULL;
    static char *username = NULL;
    static char *pass = NULL;
    static char *dbname = NULL;
    static int port = 3306;
    static char *dbsock = NULL;


    ipaddr = (char *)malloc(sizeof(char)*128);
    memset(ipaddr,0,128);
    username = (char *)malloc(sizeof(char)*256);
    memset(username,0,256);
    pass = (char *)malloc(sizeof(char)*1024);
    memset(pass,0,1024);
    dbname = (char *)malloc(sizeof(char)*128);
    memset(dbname,0,128);
    dbsock = (char *)malloc(sizeof(char)*512);
    memset(dbsock,0,512);

    strncpy(ipaddr,"127.0.0.1",127);
    strncpy(username,"root",255);
    strncpy(pass,"k7e3h8q4",1023);
    strncpy(dbname,"sysadmin",127);
    strncpy(dbsock,"/var/lib/mysql/mysql.sock",511);

    
    //创建MySQL数据库连接符。
    MYSQL *conn = NULL;
    conn = mysql_init(NULL);
    //连接到目标数据库
    mysql_real_connect(conn,ipaddr,username,pass,dbname,port,dbsock,0);

    //从数据库中获取数据
    int mysql_query_res = 0;
    mysql_query_res = mysql_query(conn,buf);
    if(mysql_query_res != 0){
        return(FALSE);
    }

    free(ipaddr);
    free(username);
    free(pass);
    free(dbname);
    free(dbsock);
    free(buf);

    return(TRUE);
}

int main(int argc,char **argv)
{
    int res =0;

    META *metadata = NULL;
    metadata = (META *)malloc(sizeof(META));
    memset(metadata,0,sizeof(META));

    metadata->metadata_type = (char *)malloc(sizeof(char)*128);
    memset(metadata->metadata_type,0,128);
    metadata->base_backup_directory = (char *)malloc(sizeof(char)*512);
    memset(metadata->base_backup_directory,0,512);
    metadata->backup_directory_name = (char *)malloc(sizeof(char)*512);
    memset(metadata->backup_directory_name,0,512);
    metadata->baseon_backup = (char *)malloc(sizeof(char)*512);
    memset(metadata->baseon_backup,0,512);
    metadata->extra_lsndir = (char *)malloc(sizeof(char)*512);
    memset(metadata->extra_lsndir,0,512);
    metadata->metadata_from_lsn =0;
    metadata->metadata_to_lsn = 0;
    metadata->metadata_last_lsn = 0;
    metadata->xtrabackup_compact = 0;

    read_xtrabackup_checkpoint_file(argv[1],argv[2],metadata);


    res = xtrabackup_write_metadata_into_db(metadata);
    
    free(metadata);

    exit(res);
}
