#include "metadata.h"

/***********************************************************************
 *Read backup meata info from MySQL Database.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static my_bool
xtrabackup_read_metadata_from_db(META *metadata)
{

    static int tmpi=0;

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
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;
    unsigned int i;

    int mysql_query_res = 0;
    mysql_query_res = mysql_query(conn,"SELECT metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir FROM sysadmin.t_xtra_backup_metadata WHERE metadata_type='full-backuped' and is_deleted = 0 ORDER BY id DESC LIMIT 1");
    if(mysql_query_res != 0){
        return(FALSE);
    }

    res = mysql_store_result(conn);
    if(res == NULL){
        return(FALSE);
    }

    row = mysql_fetch_row(res);

    for(i=0;i<mysql_num_fields(res);i++)
    {
        switch(i){
            case 0:
                sprintf(metadata->metadata_type,"%s",row[i]);
            case 1:
                metadata->metadata_from_lsn = (row[i] !=NULL? atoll(row[i]):0);
            case 2:
                metadata->metadata_to_lsn = (row[i] !=NULL? atoll(row[i]):0);
            case 3:
                metadata->metadata_last_lsn= (row[i] !=NULL? atoll(row[i]):0);
            case 4:
                metadata->xtrabackup_compact = (row[i] !=NULL? atoll(row[i]):0);
            case 5:
                snprintf(metadata->base_backup_directory,511,row[i]);
            case 6:
                snprintf(metadata->backup_directory_name,511,row[i]);
            case 7:
                snprintf(metadata->baseon_backup,511,row[i]);
            case 8:
                snprintf(metadata->extra_lsndir,511,row[i]);
            default:
                tmpi=0;

        }
    }
    return(TRUE);
}

/***********************************************************************
 *make metadata buffer.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static
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
            "compact = %d\n",
            metadata->metadata_type,
            metadata->metadata_from_lsn,
            metadata->metadata_to_lsn,
            metadata->metadata_last_lsn,
            metadata->xtrabackup_compact ==TRUE);
}


/***********************************************************************
 *create full backup directory for incremental backup.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static my_bool
create_full_backup_directory_and_metadata(META *metadata)
{

    int len = 0;
    FILE *fp = NULL;

    static char *full_backup_directory = NULL;
    static char *full_backup_metadata_file = NULL;
    static char *buf = NULL;
    full_backup_directory = (char *)malloc(sizeof(char)*512);
    memset(full_backup_directory,0,512);
    full_backup_metadata_file= (char *)malloc(sizeof(char)*512);
    memset(full_backup_metadata_file,0,512);
    buf = (char *)malloc(sizeof(char)*2048);
    memset(buf,0,2048);

    snprintf(full_backup_directory,511,"%s/%s",metadata->base_backup_directory,metadata->backup_directory_name);
    snprintf(full_backup_metadata_file,511,"%s/xtrabackup_checkpoints",full_backup_directory);


    umask(S_IWGRP | S_IWOTH);

    mkdir(full_backup_directory,0777);

    xtrabackup_print_metadata(metadata,buf,2047);

    len = strlen(buf);

    fp = fopen(full_backup_metadata_file, "w");
    if(!fp) {
        perror("fopen()");
        return(FALSE);
    }
    if (fwrite(buf, len, 1, fp) < 1) {
        fclose(fp);
        return(FALSE);
    }

    free(full_backup_directory);
    free(full_backup_metadata_file);
    free(buf);

    fclose(fp);

    return(TRUE);

}

int main(void)
{
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

    xtrabackup_read_metadata_from_db(metadata);
    
    create_full_backup_directory_and_metadata(metadata);
}
