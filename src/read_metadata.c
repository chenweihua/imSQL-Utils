#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>

static char * metadata_type;
static long metadata_from_lsn;
static long metadata_to_lsn;
static long metadata_last_lsn;
static long xtrabackup_compact;


/***********************************************************************
 *Read backup meata info from MySQL Database.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static my_bool
xtrabackup_read_metadata_from_db(void)
{

    static int tmpi=0;
    metadata_type = (char *)malloc(sizeof(char)*128);
    memset(metadata_type,0,128);

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
    mysql_query_res = mysql_query(conn,"SELECT metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact FROM sysadmin.t_xtra_backup_metadata WHERE metadata_type='full-backuped' and is_deleted = 0 ORDER BY id DESC LIMIT 1");
    if(mysql_query_res != 0){
        return(FALSE);
    }

    res = mysql_store_result(conn);
    if(res == NULL){
        return(FALSE);
    }

    if((row = mysql_fetch_row(res)) != NULL)
    {
        for(i=0;i<mysql_num_fields(res);i++)
        {
            switch(i){
                case 0:
                    strncpy(metadata_type,row[i],127);
                case 1:
                    metadata_from_lsn = row[i] !=NULL? (long)row[i]:0;
                case 2:
                    metadata_to_lsn = row[i] !=NULL? (long)row[i]:0;
                case 3:
                    metadata_last_lsn= row[i] !=NULL? (long)row[i]:0;
                case 4:
                    xtrabackup_compact = row[i] !=NULL? (long)row[i]:0;
                case 5:

                default:
                    tmpi=0;

                }
         }
    }
    return(TRUE);
}

int main(void)
{
    xtrabackup_read_metadata_from_db();
    printf("metadata_type=%s\nmetadata_from_lsn=%d\nmetadata_to_lsn=%d\n",metadata_type,metadata_from_lsn,metadata_to_lsn);
}
