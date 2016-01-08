#include "sqlite_ops.h"

/***********************************************************************
 *判断指定的表是否存在
 *返回值：存在返回1，否则返回0
 *作者：Tian, Lei [tianlei@paratera.com]
 *时间：20160203AM1046
 * **********************************************************************/
int chk_table_exists_callback(void *arg,int nr,char **values,char **names)
{
    static char *isExist;
    int i;

    isExist = (char *)arg;
    for(i=0;i<nr;i++){
        sprintf(isExist,"%s",values[i]);
    }
    return(0);
}


/***********************************************************************
 *创建hisdb数据库及其所需的数据库对象
 *返回值：正常返回0，否则返回非0
 *作者：Tian, Lei [tianlei@paratera.com]
 *时间：20160202PM1603
 **********************************************************************/
int create_hisdb_objects(char *hisdb)
{
    //定义一个变量用于保存指定的表是否存在
    static char *isTableExist;
    isTableExist = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(isTableExist,0,sizeof(char)*DFTLENGTH/8);

    //创建一个字符创指针用于保存后面的SQL语句。
    char *query = NULL;

    //创建一个sqlite3文件
    sqlite3 *db;
    sqlite3_open(hisdb,&db);

    //创建保存备份信息的表
    query = (char *)malloc(sizeof(char)*DFTLENGTH);
    memset(query,0,sizeof(char)*DFTLENGTH);

    strncpy(query,"SELECT COUNT(*) FROM sqlite_master where type='table' and name='t_xtra_backup_metadata'",sizeof(char)*DFTLENGTH-1);
    sqlite3_exec(db,query,chk_table_exists_callback,isTableExist,NULL);

    if(strcmp(isTableExist,"0") == 0 ){
        memset(query,0,sizeof(char)*DFTLENGTH);
        strncpy(query," \
                CREATE TABLE t_xtra_backup_metadata ( \
                    id integer NOT NULL PRIMARY KEY AUTOINCREMENT, \
                    metadata_type varchar(256) NOT NULL DEFAULT 'full-backuped', \
                    is_compressed int  NOT NULL DEFAULT 0, \
                    metadata_from_lsn bigint NOT NULL, \
                    metadata_to_lsn bigint NOT NULL, \
                    metadata_last_lsn bigint NOT NULL, \
                    xtrabackup_compact int DEFAULT NULL, \
                    is_deleted int NOT NULL DEFAULT '0', \
                    created_datetime datetime NOT NULL DEFAULT CURRENT_TIMESTAMP, \
                    updated_datetime datetime NOT NULL DEFAULT CURRENT_TIMESTAMP, \
                    base_backup_directory varchar(512) NOT NULL DEFAULT '/Database/Backup', \
                    backup_directory_name varchar(512) NOT NULL, \
                    baseon_backup varchar(256) DEFAULT NULL, \
                    extra_lsndir varchar(512) DEFAULT NULL \
                    )",sizeof(char)*DFTLENGTH-1);

        sqlite3_exec(db,query,NULL,NULL,NULL);
    }

    return(0);
}


/***********************************************************************
 *把xtrabackup命令生成的xtrabackup_checkpoints信息保存到sqlite3数据库中.
 *返回值：正常返回0，否则返回非0
 *作者: Tian, Lei [tianlei@paratera.com]
 *时间: 20151019PM1318
 ************************************************************************/
int
xtrabackup_write_metadata_into_db(char *hisdb,META *metadata)
{
    int cres = 0;
    sqlite3 *db;

    sqlite3_open(hisdb,&db);

    //保存查询语句的缓冲区
    char *query = NULL;
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    snprintf(query,DFTLENGTH*2-1,"INSERT INTO t_xtra_backup_metadata(metadata_type,is_compressed,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir) VALUES('%s',%d,%lld,%lld,%lld,%lld,'%s','%s','%s','%s')",metadata->metadata_type,metadata->is_compressed,(long long)metadata->metadata_from_lsn,(long long)metadata->metadata_to_lsn,(long long)metadata->metadata_last_lsn,(int)metadata->xtrabackup_compact,metadata->base_backup_directory,metadata->backup_directory_name,metadata->baseon_backup,metadata->extra_lsndir);

    sqlite3_exec(db,query,NULL,NULL,NULL);
    if(cres == 0){
        return(0);
    }
    else{
        return(1);
    }
 
}

/***********************************************************************
 *从hisdb中读取最新的一条metadata数据的回调函数.
 *返回值：成功返回0，否则返回非0
 *作者: Tian, Lei [tianlei@paratera.com]
 *时间: 20151019PM1318
*/
int 
xtrabackup_read_metadata_from_db_callback(void *arg,int nr,char **values,char **names)
{
    int i;

    META *pdata = NULL;
    pdata = (META *)arg;

    for(i=0;i<nr;i++){
        switch(i){
            case 0:
                snprintf(pdata->metadata_type,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                break;
            case 1:
                values[i] != NULL?pdata->metadata_from_lsn=atoll(values[i]):0;
                break;
            case 2:
                values[i] != NULL?pdata->metadata_to_lsn=atoll(values[i]):0;
                break;
            case 3:
                values[i] != NULL?pdata->metadata_last_lsn=atoll(values[i]):0;
                break;
            case 4:
                values[i] != NULL?pdata->xtrabackup_compact=atoi(values[i]):0;
                break;
            case 5:
                snprintf(pdata->base_backup_directory,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                break;
            case 6:
                snprintf(pdata->base_backup_directory,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                break;
            case 7:
                snprintf(pdata->baseon_backup,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                break;
            case 8:
                snprintf(pdata->extra_lsndir,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                break;
            default:
                return(1);
        }
    }
    return(0);
}

/***********************************************************************
 *从hisdb中读取最新的一条metadata数据.
 *返回值：成功返回0，否则返回非0
 *作者: Tian, Lei [tianlei@paratera.com]
 *时间: 20151019PM1318
*/
int
xtrabackup_read_metadata_from_db(char *hisdb,META *metadata)
{

    int i =0;
    int tmpi = 0;
    static int cres =0;
    char *query = NULL;

    sqlite3 *db;
    sqlite3_open(hisdb,&db);
    
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    snprintf(query,DFTLENGTH*2,"%s","SELECT metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir FROM t_xtra_backup_metadata WHERE metadata_type='full-backuped' and is_deleted = 0 ORDER BY id DESC LIMIT 1");

    sqlite3_exec(db,query,xtrabackup_read_metadata_from_db_callback,metadata,NULL);

    return(0);
}

/***********************************************************************
 *Read backup_directory_name from MySQL Database.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int
read_full_backup_name_from_db(char *hisdb,META *metadata,char *buf)
{

    xtrabackup_read_metadata_from_db(hisdb,metadata);
    
    snprintf(buf,DFTLENGTH/8,metadata->backup_directory_name);

    return(0);
}

/********************n***************************************************
 * database backup is exists?
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/

int 
database_backup_is_exists_callback(void *arg,int nr,char **values,char **names)
{
    int i;
    char *id;
    id = (char *)arg;

    sprintf(id,"%s",values[0]);
    return(0);
}

char * 
database_backup_is_exists(char *hisdb,PARA *para){
    int cres = 0;
    int fres = 0;
    char *query = NULL;
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    char *result;
    result = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    memset(result,0,sizeof(char)*DFTLENGTH/4);

    sqlite3 *db;
    sqlite3_open(hisdb,&db);

    snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from t_xtra_backup_metadata WHERE id='",para[2].content,"'");

    sqlite3_exec(db,query,database_backup_is_exists_callback,result,NULL);
                                
    return(result);
}

/********************n***************************************************
 * database backup is exists?
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int 
read_innobackup_content_from_db_callback(void *arg,int nr,char **values,char **names)
{
    int i;

    META *pdata = NULL;

    pdata = (META *)arg;
    for(i=0;i<nr;i++){
        switch(i){
            case 0:
                printf("id:  %s\n",values[i]);
                break;
            case 1:
                snprintf(pdata->metadata_type,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                printf("metadata_type: %s\n",values[i]);
                break;
            case 2:
                printf("metadata_from_lsn: %s\n",values[i]);
                break;
            case 3:
                printf("metadata_to_lsn: %s\n",values[i]);
                break;
            case 4:
                printf("metadata_last_lsn: %s\n",values[i]);
                break;
            case 5:
                printf("xtrabackup_compact: %s\n",values[i]);
                break;
            case 6:
                printf("is_deleted: %s\n",values[i]);
                break;
            case 7:
                printf("created_datetime: %s\n",values[i]);
                break;
            case 8:
                printf("updated_datetime: %s\n",values[i]);
                break;
            case 9:
                snprintf(pdata->base_backup_directory,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                printf("base_backup_directory: %s\n",values[i]);
                break;
            case 10:
                snprintf(pdata->backup_directory_name,DFTLENGTH/8-1,"%s",values[i] !=NULL?values[i]:"");
                printf("backup_directory_name: %s\n",values[i]);
                break;
            case 11:
                snprintf(pdata->baseon_backup,DFTLENGTH/8-1,"%s",values[i] != NULL?values[i]:"");
                printf("baseon_backup:  %s\n",values[i]);
                break;
            case 12:
                printf("extra_lsndir:  %s\n",values[i]);
                break;
            default:
                printf("Err\n");
        }
    }

}

int 
read_innobackup_content_from_db(char *hisdb,PARA *para,META *metadata){
    int cres = 0;
    int fres = 0;
    int pres = 0;
    int i = 0;
    char *backup_is_exists = NULL;
    backup_is_exists = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(backup_is_exists,0,sizeof(char)*DFTLENGTH/2);

    int num_rows = 0;
    char *query = NULL;
    
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    sqlite3 *db;
    sqlite3_open(hisdb,&db);
    
    switch(para->argclen){
        case 2:
            print_history_help();
            break;
        case 3:
            print_history_help();
            break;
        case 4:
            backup_is_exists = database_backup_is_exists(hisdb,para);
            if(strcmp(backup_is_exists,"1") == 0){
                if(strstr("into",para[3].content)){
                    if(strlen(para[4].content) != 0 ){
                        snprintf(query,DFTLENGTH*2,"%s%s","select id,metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,is_deleted,created_datetime,updated_datetime,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir from t_xtra_backup_metadata where id =",para[2].content);
                        sqlite3_exec(db,query,read_innobackup_content_from_db_callback,metadata,NULL);
                    }
                }
            }
            break;
        case 5:
            break;
        case 6:
            backup_is_exists = database_backup_is_exists(hisdb,para);
            if(strcmp(backup_is_exists,"1") == 0){
                if(strstr("from",para[3].content)){
                    if(strlen(para[4].content) != 0 ){
                        if(strstr("into",para[5].content)){
                            if(strlen(para[6].content) != 0){
                                snprintf(query,DFTLENGTH*2,"%s%s","select id,metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,is_deleted,created_datetime,updated_datetime,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir from t_xtra_backup_metadata where id =",para[2].content);
                                sqlite3_exec(db,query,read_innobackup_content_from_db_callback,metadata,NULL);
                            }
                        }
                    }
                }
            }
            break;
        default:
            printf("Restore Err\n");
    }

    return(0);
}

/********************n***************************************************
 * database list
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int get_database_list(void){
    return(0);
}



/***********************************************************************
 * some database operation history.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int operate_database_history(PARA *para){
    return(0);
}

