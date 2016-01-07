#include "pdb.h"

/***********************************************************************
 *打印出pdb命令的总帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
***********************************************************************/
void print_main_help(void)
{
    char *help_msgs = "Usage: \n \
\tpdb {backup|restore|list|shell|help} \n \
\nSample: \n \
\tpdb backup {all|db} {NULL|dbname} {full|incremental} {offline|online} [compress] {to 'path'} \n \
\tpdb restore {backup_id} [from 'path'] [into 'path'] \n \
\tpdb list history [backup|restore|archive_logs|event_schedule] [begin 'timestamp'] [end 'tiemstamp'] [detail] \n \
\tpdb shell \n \
";
    printf("%s",help_msgs);
}

/***********************************************************************
 *打印出pdb backup备份命令的版主信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_backup_help(void)
{
    char *help_msgs = "Usage: \n \
\tpdb backup {all|db} {NULL|dbname} {full|incremental} {offline|online} [compress] {to 'path'} \n \
\nSample:\n \
\tpdb backup all full offline  to \'/dbbackup\' \n \
\tpdb backup all full offline compress to \'/dbbackup\' \n \
\tpdb backup all full online to \'/dbbackup\' \n \
\tpdb backup all full online compress to '/dbbackup' \n \
\tpdb backup all incremental online to '/dbbackup' \n \
\tpdb backup all incremental online compress to '/dbbackup' \n \
\tpdb backup db basedb full online to '/dbbackup' \n \
\tpdb backup db basedb full online compress to '/dbbackup' \n \
\tpdb backup db basedb incremental online to '/dbbackup' \n \
\tpdb backup db basedb incremental online compress to '/dbbackup' \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *打印pdb restore恢复命令的帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_restore_help(void)
{
    char *help_msgs = "Usage: \
\tpdb restore {backup_id} [from 'path'] [into 'path'] \n \
\nSample:\n \
\t1.get backup list \n \
\t\t#pdb list history backup \n \
\t\tOR \n \
\t\t#pdb list history backup begin '2015-01-01 00:00:00' end '2015-01-07 23:59:59' \n \
\t\tYou Can get the backup id number. \n \
\t2.restore database by backup id. \n \
\t\t#pdb restore 1 from '/dbbackup' into '/Database' \n \
\t\tOR \n \
\t\t#pdb restore 1 \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *打印pdb history历史命令的帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_history_help(void)
{
    char *help_msgs = "Usage: \
\tpdb list history [backup|restore|archive_logs|event_schedule] [begin 'timestamp'] [end 'tiemstamp'] [detail] \n \
\nSample: \n \
\t\t#pdb list history backup \n \
\t\tOR \n \
\t\t#pdb list history backup begin '2014-01-01 00:00:00' end '2015-12-31 23:59:59' \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *打印出pdb shell命令的帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_shell_help(void)
{
    char *help_msgs = "Usage: \
\tpdb shell \n";
    printf("%s",help_msgs);
}

/***********************************************************************
 *parse database connection params informations,save informations into DBP struct.
 *@return TRUE on success,FALSE on failure.
 *Author:  Tian, Lei [tianlei@paratera.com]
 *Date:    20151019PM1318
 ************************************************************************/
int parse_database_conn_params(char *filename,DBP *dbp){
    FILE *fp = NULL;
    int res = 0;

    fp = fopen(filename,"r");
    if(fp == NULL){
        perror("fopen()");
        res = 1;
        return(res);
    }

    if(fscanf(fp,"host = %s\n",dbp->host) != 1){
        res = 21;
        goto end;
    }
    if(fscanf(fp,"user = %s\n",dbp->user) != 1){
        res = 22;
        goto end;
    }
    if(fscanf(fp,"socket = %s\n",dbp->socket) != 1){
        res = 23;
        goto end;
    }
    if(fscanf(fp,"pass = %s\n",dbp->pass) != 1){
        res = 24;
        goto end;
    }
    if(fscanf(fp,"port = %d\n",&(dbp->port)) != 1){
        res = 25;
        goto end;
    }

end:
    fclose(fp);
    return(res);
}



/**********************************************************************
 *从xtrabackup命令备份生成的metadata文件中读取meta数据，然后把这些数据保存到META结构体中.
 *返回值：正常返回0，否则返回非零
 *作者: Tian, Lei [tianlei@paratera.com]
 *时间: 20151019PM1713
 *********************************************************************/
static my_bool
read_xtrabackup_checkpoint_file(char *extra_lsndir,META *metadata)
{
    //创建一个文件描述符用于读取xtrabackup命令生成的xtrabackup_checkpoints文件 
    FILE *fp = NULL;
    my_bool  r = TRUE;
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


/***********************************************************************
 *把/etc/sysconfig/pdb/innobackupex文件的内容保存到INNOBAK结构体中.
 *返回值：正常返回0，否则返回非零
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
int parse_innobackupex_params(char *filename,INNOBAK *innobak){
    FILE *fp = NULL;
    int res = 0;
    char *ikey;
    char *ivalues;

    ikey = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    ivalues = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    memset(ikey,0,DFTLENGTH/4);
    memset(ivalues,0,DFTLENGTH/4);

    fp = fopen(filename,"r");
    if(fp == NULL){
        perror("fopen()");
        res = 1;
        return(res);
    }

    if(fscanf(fp,"innobak_bin = %s\n",innobak->innobak_bin) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"parallel = %d\n",&(innobak->parallel)) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"throttle = %d\n",&(innobak->throttle)) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"use_memory = %s\n",innobak->use_memory) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"encrypt = %s\n",innobak->encrypt ) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"encrypt_key_file = %s\n",innobak->encrypt_key_file ) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"stream = %s\n",innobak->stream ) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"extra_lsndir = %s\n",innobak->extra_lsndir) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"hostname = %s\n",innobak->hostname ) != 1){
        res = 35;
        goto end;
    }
    innobak->compress_threads = innobak->parallel*2;
end:
    fclose(fp);
    return(res);
}

/***********************************************************************
 *判断指定的表是否存在
 *返回值：存在返回1，否则返回0
 *作者：Tian, Lei [tianlei@paratera.com]
 *时间：20160203AM1046
 * **********************************************************************/
static int chk_table_exists(void *arg,int nr,char **values,char **names)
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
static int create_hisdb_objects(char *hisdb)
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
    sqlite3_exec(db,query,chk_table_exists,isTableExist,NULL);

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
static my_bool
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
int xtrabackup_read_metadata_from_db_callback(void *arg,int nr,char **values,char **names)
{
    int i;
    char *buf =NULL;

    META *pdata = NULL;
    pdata = (META *)arg;

    buf = (char *)malloc(sizeof(char)*DFTLENGTH);
    memset(buf,0,sizeof(char)*DFTLENGTH);


    buf = (char *)arg;
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
                pdata->base_backup_directory;
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
static my_bool
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

    return(TRUE);
}

/***********************************************************************
 *Read backup_directory_name from MySQL Database.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static my_bool
read_full_backup_name_from_db(char *hisdb,META *metadata,char *buf)
{

    xtrabackup_read_metadata_from_db(hisdb,metadata);
    
    snprintf(buf,DFTLENGTH/8,metadata->backup_directory_name);

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
            metadata->xtrabackup_compact ==TRUE);
}


/***********************************************************************
 *create full backup directory for incremental backup.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static my_bool
create_full_backup_directory_and_metadata(INNOBAK *innobak,META *metadata)
{

    int len = 0;
    FILE *fp = NULL;

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

/********************n***************************************************
 * database is exists?
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int database_is_exists(PARA *para){
    return(1);
}

/********************n***************************************************
 * database backup is exists?
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/

int database_backup_is_exists_callback(void *arg,int nr,char **values,char **names)
{
    int i;
    char *id;
    id = (char *)arg;

    sprintf(id,"%s",values[0]);
    return(0);
}

char * database_backup_is_exists(char *hisdb,PARA *para){
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
int read_innobackup_content_from_db_callback(void *arg,int nr,char **values,char **names)
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

int read_innobackup_content_from_db(char *hisdb,PARA *para,META *metadata){
    int cres = 0;
    int fres = 0;
    int pres = 0;
    int i = 0;
    char *backup_is_exists = NULL;
    backup_is_exists = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(backup_is_exists,0,sizeof(char)*DFTLENGTH/2);

    my_ulonglong num_rows = 0;
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


/********************n***************************************************
 * backup database operation.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int backup_database(PARA *para,DBP *dbp,INNOBAK *innobak,META *meta){
    struct tm *ptr;
    time_t ct;
    char *timestamp_buf = NULL;

    MYSQL_RES *res = NULL;
    MYSQL_ROW row;
    unsigned int i,cres,pres;



    char *query = NULL;                 //缓冲查询语句
    char *iconn = NULL;                 //保存连接数据库的详细信息
    char *istream = NULL;               //保存innobackupex备份方法
    char *icompress = NULL;             //保存是否压缩
    char *iuse_memory = NULL;           //保存innobackupex内存占用
    char *icompress_threads = NULL;     //保存压缩线程
    char *iparallel = NULL;             //保存并行参数
    char *ithrottle = NULL;             //保存磁盘iops参数
    char *iencrypt = NULL;              //保存加密参数
    char *iencrypt_key_file = NULL;     //保存加密文件路径
    char *iextra_lsndir = NULL;
    char *ibackup_file_name = NULL;
    char *innobackupex = NULL;
    char *iincremental = NULL;
    char *iincremental_lsn = NULL;
    char *ibaseon_backup = NULL;

    timestamp_buf = (char *)malloc(sizeof(char)*DFTLENGTH/20);
    memset(timestamp_buf,0,DFTLENGTH/20);

    innobackupex = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    iconn = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    istream = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    icompress = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iuse_memory = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    icompress_threads= (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iparallel = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    ithrottle = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iencrypt = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iencrypt_key_file = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    iextra_lsndir = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    ibackup_file_name= (char *)malloc(sizeof(char)*DFTLENGTH);
    iincremental  = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    iincremental_lsn = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    ibaseon_backup = (char *)malloc(sizeof(char)*DFTLENGTH/8);

    memset(innobackupex,0,DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);
    memset(iconn,0,DFTLENGTH*2);
    memset(istream,0,DFTLENGTH/4);
    memset(icompress,0,DFTLENGTH/4);
    memset(iuse_memory,0,DFTLENGTH/4);
    memset(iparallel,0,DFTLENGTH/4);
    memset(ithrottle,0,DFTLENGTH/4);
    memset(icompress_threads,0,DFTLENGTH/4);
    memset(iencrypt,0,DFTLENGTH/4);
    memset(iencrypt_key_file,0,DFTLENGTH/2);
    memset(iextra_lsndir,0,DFTLENGTH/2);
    memset(ibackup_file_name,0,DFTLENGTH);
    memset(iincremental,0,DFTLENGTH/8);
    memset(iincremental_lsn,0,DFTLENGTH*2);
    memset(ibaseon_backup,0,DFTLENGTH/8);

    /*
       获取必须的参数
       */
    //获取数据库参数
    pres = parse_database_conn_params(pdb_conn_info,dbp);
    //获取innobackupex参数
    parse_innobackupex_params(inno_conn_info,innobak);
    snprintf(istream,DFTLENGTH/4,"--stream=%s",innobak->stream);
    snprintf(iparallel,DFTLENGTH/4,"--parallel=%d",innobak->parallel);
    snprintf(icompress_threads,DFTLENGTH/4,"--compress-threads=%d",innobak->compress_threads);
    snprintf(iencrypt,DFTLENGTH/4,"--encrypt=%s",innobak->encrypt);
    snprintf(iencrypt_key_file,DFTLENGTH/2,"--encrypt-key-file=%s",innobak->encrypt_key_file);
    snprintf(ithrottle,DFTLENGTH/4,"--throttle=%d",innobak->throttle);
    snprintf(iuse_memory,DFTLENGTH/4,"--use_memory=%s",innobak->use_memory);
    snprintf(iconn,DFTLENGTH*2,"--host=%s --user=%s --password=%s --port=%d",dbp->host,dbp->user,dbp->pass,dbp->port);
    snprintf(iextra_lsndir,DFTLENGTH/2,"--extra-lsndir=%s",innobak->extra_lsndir);

    switch(para->argclen){
        case 2:
            print_backup_help();
            return(220);
            break;
        case 3:
            print_backup_help();
            return(230);
            break;
        case 4:
            print_backup_help();
            return(240);
            break;
        case 5:
            print_backup_help();
            return(250);
            break;
        case 6:
            if(strstr("all",para[2].content)){
                //pdb backup all full offline  to '/dbbackup'
                //pdb backup all full online to '/dbbackup'
                if(strstr("full",para[3].content)){
                    if(strstr("offline",para[4].content)){
                        if(strstr("to",para[5].content)){
                            if(strlen(para[5].content) != 0){
                                //pdb backup all full offline  to '/dbbackup'
                                printf("Please backup db manual\n");
                            }
                            else{
                                print_backup_help();
                                return(261);
                            }
                        }else{
                            print_backup_help();
                            return(262);
                        }
                    }
                    else if(strstr("online",para[4].content)){
                        if(strstr("to",para[5].content)){
                            if(strlen(para[6].content) != 0){
                                //pdb backup all full online to '/dbbackup'
                                /*得出当前时间戳*/ 
                                ct = time(NULL);
                                ptr = localtime(&ct);
                                strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s","all");
                                snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",0);
                                snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",0);
                                snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[6].content);

                                snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                /*拼接innodbbackupex 命令行*/
                                snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,para[6].content,">",para[6].content,ibackup_file_name);
                                system(innobackupex);

                                read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                xtrabackup_write_metadata_into_db(hisdb,meta);

                            }
                            else{
                                print_backup_help();
                                return(263);
                            }
                        }
                        else{
                            print_backup_help();
                            return(264);
                        }
                    }
                    else{
                        print_backup_help();
                        return(265);
                    }
                }else if(strstr("incremental",para[3].content)){
                    //pdb backup all incremental online to '/dbbackup'
                    if(strstr("online",para[4].content)){
                        if(strstr("to",para[5].content)){
                            if(strlen(para[6].content) != 0){
                                /*得出当前时间戳*/ 
                                ct = time(NULL);
                                ptr = localtime(&ct);
                                strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s","all");
                                snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",0);
                                snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[6].content);

                                snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                xtrabackup_read_metadata_from_db(hisdb,meta);

                                snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                /*拼接innodbbackupex 命令行*/
                                snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,iincremental,iincremental_lsn,para[6].content,">",para[6].content,ibackup_file_name);
                                system(innobackupex);

                                read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                xtrabackup_write_metadata_into_db(hisdb,meta);

                                printf("onlie incremental backup all\n");
                            }
                            else{
                                print_backup_help();
                                return(266);
                            }
                        }
                        else{
                            print_backup_help();
                            return(267);
                        }
                    }else{
                        print_backup_help();
                        return(268);
                    }
                }else{
                    print_backup_help();
                    return(269);
                } 
            }
            else{
                print_backup_help();
                return(260);
            }
            break;
        case 7:
            /*
               pdb backup db basedb full online to '/dbbackup' 
               pdb backup db basedb incremental online to '/dbbackup' 
               */
            if((strstr(para[2].content,"database") || strstr(para[2].content,"db"))){
                if((strstr("full",para[3].content) == NULL || strstr("incremental",para[3].content) == NULL ||strstr("online",para[3].content) == NULL ||strstr("to",para[3].content) == NULL) && (strlen(para[3].content) != 0)){
                    if(strstr(para[4].content,"full")){
                        if(strstr(para[5].content,"online")){
                            if(strstr(para[6].content,"to")){
                                if(strlen(para[7].content) != 0){
                                    cres = database_is_exists(para);
                                    if(cres >0){
                                        //pdb backup db basedb full online to '/dbbackup' 
                                        /*得出当前时间戳*/ 
                                        ct = time(NULL);
                                        ptr = localtime(&ct);
                                        strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                        snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                        snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                        snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",0);
                                        snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                        snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",0);
                                        snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                        snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                        snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                        snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);

                                        snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                        snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                        /*拼接innodbbackupex 命令行*/
                                        snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,para[7].content,">",para[7].content,ibackup_file_name);
                                        system(innobackupex);

                                        read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                        xtrabackup_write_metadata_into_db(hisdb,meta);

                                    }
                                    else{
                                        printf("No %s\n",para[3].content);
                                        return(272);
                                    }
                                }else{
                                    print_backup_help();
                                    return(273);
                                }
                            }
                            else{
                                print_backup_help();
                                return(274);
                            }
                        }
                        else{
                            print_backup_help();
                            return(275);
                        }
                    }
                    //pdb backup db basedb incremental online to '/dbbackup'
                    else if(strstr(para[4].content,"incremental")){
                        if(strstr(para[5].content,"online")){
                            if(strstr(para[6].content,"to")){
                                if(strlen(para[7].content) != 0){
                                    cres = database_is_exists(para);
                                    if(cres >0){
                                        /*得出当前时间戳*/ 
                                        ct = time(NULL);
                                        ptr = localtime(&ct);
                                        strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                        snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                        snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                        snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                        snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                        snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",0);
                                        snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                        snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                        snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                        snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);

                                        snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                        snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                        read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                        xtrabackup_read_metadata_from_db(hisdb,meta);

                                        snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                        snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                        /*拼接innodbbackupex 命令行*/
                                        snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,iincremental,iincremental_lsn,para[7].content,">",para[7].content,ibackup_file_name);
                                        system(innobackupex);

                                        read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                        snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                        snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                        xtrabackup_write_metadata_into_db(hisdb,meta);

                                    }
                                    else{
                                        printf("No %s\n",para[3].content);
                                        return(272);
                                    }
                                }
                                else{
                                    print_backup_help();
                                    return(273);
                                }
                            }
                            else{
                                print_backup_help();
                                return(274);
                            }
                        }
                        else{
                            print_backup_help();
                            return(275);
                        }
                    }
                    else{
                        print_backup_help();
                        return(276);
                    }
                } 
                else {
                    print_backup_help();
                    return(278);
                }
            }
            else if(strstr("all",para[2].content)){
                /*
                   pdb backup all full offline compress to '/dbbackup' 
                   pdb backup all full online compress to '/dbbackup' 
                   pdb backup all incremental online compress to '/dbbackup' 
                   */
                if(strstr("full",para[3].content)){
                    if(strstr("offline",para[4].content)){
                        if(strstr("compress",para[5].content)){
                            if(strstr("to",para[6].content)){
                                if(strlen(para[7].content) != 0){
                                    //pdb backup all full offline compress to '/dbbackup' 
                                    printf("pdb backup all full offline compress to /dbbackup \n");
                                }
                                else{
                                    print_backup_help();
                                    return(279);
                                }
                            }else{
                                print_backup_help();
                                return(2710);
                            }
                        }
                        else{
                            print_backup_help();
                            return(2711);
                        }
                    }
                    else if(strstr("online",para[4].content)){
                        if(strstr("compress",para[5].content)){
                            if(strstr("to",para[6].content)){
                                if(strlen(para[7].content) != 0){
                                    //pdb backup all full online compress to '/dbbackup' 
                                    /*得出当前时间戳*/ 
                                    ct = time(NULL);
                                    ptr = localtime(&ct);
                                    strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                    snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s","all");
                                    snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                    snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",0);
                                    snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                    snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",1);
                                    snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                    snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                    snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");
                                    snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);
                                    snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                    snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);
                                    snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,para[7].content,">",para[7].content,ibackup_file_name);
                                    system(innobackupex);

                                    meta->is_compressed = 1;

                                    read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                    xtrabackup_write_metadata_into_db(hisdb,meta);

                                }
                                else{
                                    print_backup_help();
                                    return(2712);
                                }
                            }
                            else{
                                print_backup_help();
                                return(2713);
                            }
                        }
                        else{
                            print_backup_help();
                            return(2714);
                        }
                    }
                    else{
                        print_backup_help();
                        return(2715);
                    }
                }
                else if(strstr("incremental",para[3].content)){
                    if(strstr("online",para[4].content)){
                        if(strstr("compress",para[5].content)){
                            if(strstr("to",para[6].content)){
                                if(strlen(para[7].content) != 0){
                                    //pdb backup all incremental online compress to '/dbbackup' 
                                    /*得出当前时间戳*/ 
                                    ct = time(NULL);
                                    ptr = localtime(&ct);
                                    strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                    snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s","all");
                                    snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                    snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                    snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                    snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",1);
                                    snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                    snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                    snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                    snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);

                                    snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                    snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);


                                    read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                    xtrabackup_read_metadata_from_db(hisdb,meta);

                                    snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                    snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                    /*拼接innodbbackupex 命令行*/
                                    snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,iincremental,iincremental_lsn,para[7].content,">",para[7].content,ibackup_file_name);
                                    system(innobackupex);

                                    read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                    snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                    snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);
                                    
                                    meta->is_compressed = 1;

                                    xtrabackup_write_metadata_into_db(hisdb,meta);

                                    return(0);
                                }
                                else{
                                    printf("Backup Directory Path is NULL\n");
                                    print_backup_help();
                                    return(2720);
                                }
                            }
                            else{
                                print_backup_help();
                                return(2719);
                            }
                        }
                        else{
                            print_backup_help();
                            return(2718);
                        }
                    }
                    else{
                        print_backup_help();
                        return(2717);
                    }
                }
                else{
                    print_backup_help();
                    return(2716);
                }
            }
            else
            {
                print_backup_help();
                return(2716);
            }
            break;
        case 8:
            /*
               pdb backup db basedb full online compress to '/dbbackup' 
               pdb backup db basedb incremental online compress to '/dbbackup' 
               */
            if((strstr(para[2].content,"database") || strstr(para[2].content,"db"))){
                if((strstr("full",para[3].content) == NULL || strstr("incremental",para[3].content) == NULL ||strstr("online",para[3].content) == NULL ||strstr("to",para[3].content) == NULL) && (strlen(para[3].content) != 0)){
                    if(strstr(para[4].content,"full")){
                        if(strstr(para[5].content,"online")){
                            if(strstr(para[6].content,"compress")){
                                if(strstr("to",para[7].content)){
                                    if(strlen(para[8].content) != 0){
                                        cres = database_is_exists(para);
                                        if(cres >0){
                                            //pdb backup db basedb full online compress to '/dbbackup' 
                                            /*得出当前时间戳*/ 
                                            ct = time(NULL);
                                            ptr = localtime(&ct);
                                            strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                            snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                            snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                            snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",0);
                                            snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                            snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                            snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");
                                            snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[8].content);
                                            snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                            snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);
                                            snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,para[8].content,">",para[8].content,ibackup_file_name);
                                            system(innobackupex);

                                            meta->is_compressed = 1;

                                            read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                            xtrabackup_write_metadata_into_db(hisdb,meta);

                                        }
                                        else{
                                            printf("No %s\n",para[3].content);
                                            return(282);
                                        }
                                    }else{
                                        print_backup_help();
                                        return(285);

                                    }
                                }
                                else{
                                    print_backup_help();
                                    return(283);
                                }
                            }
                            else{
                                print_backup_help();
                                return(284);
                            }
                        }
                        else{
                            print_backup_help();
                            return(285);
                        }
                    }
                    else if(strstr(para[4].content,"incremental")){
                        if(strstr("online",para[5].content)){
                            if(strstr("compress",para[6].content)){
                                if(strstr("to",para[7].content)){
                                    if(strlen(para[8].content) != 0){
                                        cres = database_is_exists(para);
                                        if(cres >0){
                                            //pdb backup db basedb incremental online compress to '/dbbackup' 
                                            /*得出当前时间戳*/ 
                                            ct = time(NULL);
                                            ptr = localtime(&ct);
                                            strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                            snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                            snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                            snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                            snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                            snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                            snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                            snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[8].content);

                                            snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                            snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                            read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                            xtrabackup_read_metadata_from_db(hisdb,meta);

                                            snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                            snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                            /*拼接innodbbackupex 命令行*/
                                            snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,iincremental,iincremental_lsn,para[8].content,">",para[8].content,ibackup_file_name);
                                            system(innobackupex);

                                            read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                            snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                            snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                            meta->is_compressed = 1;

                                            xtrabackup_write_metadata_into_db(hisdb,meta);

                                        }
                                        else{
                                            printf("No %s\n",para[3].content);
                                            return(289);
                                        }

                                    }
                                    else{
                                        print_backup_help();
                                        return(2810);
                                    }
                                }
                                else{
                                    print_backup_help();
                                    return(2811);
                                }
                            }
                            else{
                                print_backup_help();
                                return(2812);
                            }

                        }
                        else{
                            print_backup_help();
                            return(2813);
                        }
                    }
                    else{
                        print_backup_help();
                        return(2814);
                    }
                }
                else
                {
                    print_backup_help();
                    return(2816);
                }
            }
            else{
                print_backup_help();
                return(2817);
            }
            break;
        default:
            print_backup_help();
            return(29);
    }
    return(0);
}


/***********************************************************************
 * restore database operation.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
 */
int restore_database(DBP *dbp,PARA *para,META *metadata){

    MYSQL_RES *res = NULL;
    MYSQL_ROW row;

    char * backup_is_exists = NULL;
    backup_is_exists = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(backup_is_exists,0,sizeof(char)*DFTLENGTH/2);

    int pres = 0;

    char *restore_ops = NULL;
    char *restore_applog = NULL;
    int restore_ops_res = 0;
    int applog_ops_res = 0;

    char *incremental_restore_path = NULL;
    char *remove_incremental_path_ops = NULL;
    char *decompress_ops = NULL;
    incremental_restore_path = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    remove_incremental_path_ops = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    decompress_ops = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(incremental_restore_path,0,DFTLENGTH*2);
    memset(remove_incremental_path_ops,0,DFTLENGTH*2);
    memset(decompress_ops,0,DFTLENGTH*2);

    restore_ops = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    restore_applog = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(restore_ops,0,DFTLENGTH*2);
    memset(restore_applog,0,DFTLENGTH*2);

    //获取数据库参数
    pres = parse_database_conn_params(pdb_conn_info,dbp);

    switch(para->argclen){
        case 2:
            print_restore_help();
            break;
        case 3:
            print_restore_help();
            break;
        case 4:
            backup_is_exists = database_backup_is_exists(hisdb,para);
            if(strcmp(backup_is_exists,"1") == 0){
                if(strstr("into",para[3].content)){
                    if(strlen(para[4].content) != 0){
                        //pdb restore 100 into /Database
                        read_innobackup_content_from_db(hisdb,para,metadata);
                        if(strstr(metadata->metadata_type,"full-backuped")){
                            printf("full-backuped\n");
                            snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",metadata->base_backup_directory,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",para[4].content);
                            restore_ops_res = system(restore_ops);
                            if(restore_ops_res == 0){
                                if(metadata->is_compressed = 1){
                                    printf("Decompress Tablespaces\n");
                                    snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[4].content);
                                    system(decompress_ops);
                                }
                                snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[4].content);
                                applog_ops_res = system(restore_applog);
                                if(applog_ops_res == 0){
                                    printf("Apply Logs Success\n");
                                }
                                else{
                                    printf("Apply Logs Failure\n");
                                }
                            }
                            else{
                                printf("Restore Failure\n");
                            }
                        }
                        else if(strstr(metadata->metadata_type,"incremental")){
                            printf("incremental\n");
                            //restore full backup .
                            snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",metadata->base_backup_directory,metadata->baseon_backup,"/usr/bin/xbstream","-x","-C",para[4].content);
                            printf("restore full backup %s/%s\n",metadata->base_backup_directory,metadata->baseon_backup);
                            restore_ops_res = system(restore_ops);
                            if(restore_ops_res == 0){
                                //Decompress Tablespaces.
                                if(metadata->is_compressed = 1){
                                    printf("Decompress Tablespaces\n");
                                    snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[4].content);
                                    system(decompress_ops);
                                }
                                snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[4].content);
                                applog_ops_res = system(restore_applog);
                                if(applog_ops_res == 0){
                                    printf("Apply Full Backup Logs Success\n");
                                    //restore incremental backup.
                                    memset(restore_ops,0,DFTLENGTH*2);
                                    snprintf(incremental_restore_path,DFTLENGTH*2-1,"%s/%s",para[4].content,"incr");
                                    mkdir(incremental_restore_path,0777);
                                    snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",metadata->base_backup_directory,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",incremental_restore_path);
                                    printf("restore incremental backup %s/%s\n",metadata->base_backup_directory,metadata->backup_directory_name);
                                    restore_ops_res = system(restore_ops);
                                    if(restore_ops_res == 0){
                                        //Decompress Tablespaces.
                                        if(metadata->is_compressed = 1){
                                            printf("Decompress Tablespaces\n");
                                            snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",incremental_restore_path);
                                            system(decompress_ops);
                                        }
                                        snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s %s%s","/usr/bin/innobackupex","--apply-log --redo-only",para[4].content,"--incremental-dir=",incremental_restore_path);
                                        applog_ops_res = system(restore_applog);
                                        if(applog_ops_res == 0){
                                            printf("Apply Incremental Backup Logs Success\n");
                                            snprintf(remove_incremental_path_ops,DFTLENGTH*2-1,"%s %s","rm -fr",incremental_restore_path);
                                            system(remove_incremental_path_ops);
                                        }
                                        else{
                                            printf("Apply Incremental Backup Logs Failure\n");
                                        }
                                    }
                                    else{
                                        printf("Restore Failure\n");
                                    }

                                }
                                else{
                                    printf("Apply Logs Failure\n");
                                }
                            }
                            else{
                                printf("Restore Failure\n");
                            }
                        }
                        else{
                            printf("metadata_type != full-backuped or incremental\n");
                            return(41);
                        }
                    }
                    else{
                        printf("Directory Error\n");
                        print_restore_help();
                    }
                }
                else{
                    print_restore_help();
                }
            }
            else{
                printf("No backup id %d\n",para[2].content);
                print_restore_help();
            }
            break;
        case 5:
            print_restore_help();
            break;
        case 6:
            backup_is_exists = database_backup_is_exists(hisdb,para);
            if(strcmp(backup_is_exists,"1") == 0){
                if(strstr("from",para[3].content)){
                    if(strlen(para[4].content) != 0){
                        if(strstr("into",para[5].content)){
                            if(strlen(para[6].content) != 0){
                                //pdb restore 100 from /root/backup into /Database
                                read_innobackup_content_from_db(hisdb,para,metadata);
                                if(strstr(metadata->metadata_type,"full-backuped")){
                                    printf("full-backuped\n");
                                    snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",para[4].content,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",para[6].content);
                                    restore_ops_res = system(restore_ops);
                                    if(restore_ops_res == 0){
                                        //Decompress Tablespaces.
                                        if(metadata->is_compressed = 1){
                                            printf("Decompress Tablespaces\n");
                                            snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[6].content);
                                            system(decompress_ops);
                                        }
                                        snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[6].content);
                                        applog_ops_res = system(restore_applog);
                                        if(applog_ops_res == 0){
                                            printf("Apply Logs Success\n");
                                        }
                                        else{
                                            printf("Apply Logs Failure\n");
                                        }
                                    }
                                    else{
                                        printf("Restore Failure\n");
                                    }
                                }
                                else if(strstr(metadata->metadata_type,"incremental")){
                                    printf("incremental\n");
                                    //restore full backup .
                                    snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",para[4].content,metadata->baseon_backup,"/usr/bin/xbstream","-x","-C",para[6].content);
                                    printf("restore full backup %s/%s\n",para[4].content,metadata->baseon_backup);
                                    restore_ops_res = system(restore_ops);
                                    if(restore_ops_res == 0){

                                        //Decompress Tablespaces.
                                        if(metadata->is_compressed = 1){
                                            printf("Decompress Tablespaces\n");
                                            snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[6].content);
                                            system(decompress_ops);
                                        }
                                        snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[6].content);
                                        applog_ops_res = system(restore_applog);
                                        if(applog_ops_res == 0){
                                            printf("Apply Full Backup Logs Success\n");
                                            //restore incremental backup.
                                            memset(restore_ops,0,DFTLENGTH*2);
                                            snprintf(incremental_restore_path,DFTLENGTH*2-1,"%s/%s",para[6].content,"incr");
                                            mkdir(incremental_restore_path,0777);
                                            snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",para[4].content,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",incremental_restore_path);
                                            printf("restore incremental backup %s/%s\n",para[4].content,metadata->backup_directory_name);
                                            restore_ops_res = system(restore_ops);
                                            if(restore_ops_res == 0){
                                                //Decompress Tablespaces.
                                                if(metadata->is_compressed = 1){
                                                    printf("Decompress Tablespaces\n");
                                                    snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",incremental_restore_path);
                                                    system(decompress_ops);
                                                }
                                                snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s %s%s","/usr/bin/innobackupex","--apply-log --redo-only",para[6].content,"--incremental-dir=",incremental_restore_path);
                                                applog_ops_res = system(restore_applog);
                                                if(applog_ops_res == 0){
                                                    printf("Apply Incremental Backup Logs Success\n");
                                                    snprintf(remove_incremental_path_ops,DFTLENGTH*2-1,"%s %s","rm -fr",incremental_restore_path);
                                                    system(remove_incremental_path_ops);
                                                }
                                                else{
                                                    printf("Apply Incremental Backup Logs Failure\n");
                                                }
                                            }
                                            else{
                                                printf("Restore Failure\n");
                                            }

                                        }
                                        else{
                                            printf("Apply Logs Failure\n");
                                        }
                                    }
                                    else{
                                        printf("Restore Failure\n");
                                    }
                                }
                                else{
                                    printf("metadata_type != full-backuped or incremental\n");
                                    return(41);
                                }
                            }
                            else{
                                printf("Directory Error\n");
                                print_restore_help();
                            }
                        }
                        else{
                            print_restore_help();
                        }
                    }
                    else{
                        printf("Directory Error\n");
                        print_restore_help();
                    }
                }
                else{
                    print_restore_help();
                }
            }
            else{
                printf("No backup id %d\n",para[2].content);
                print_restore_help();
            }
            break;
        default:
            printf("Err Restore\n");
    }
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


/***********************************************************************
 * some database operation history.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
 */
static int list_backup_history_callbak(void *arg,int nr,char **values,char **names)
{
    int i;
        
    for(i = 0;i<nr;i++ ){
            switch(i){
                case 0:
                    printf("id:  %s\n",values[i]);
                    break;
                case 1:
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
                    printf("base_backup_directory: %s\n",values[i]);
                    break;
                case 10:
                    printf("backup_directory_name: %s\n",values[i]);
                    break;
                case 11:
                    printf("baseon_backup:  %s\n",values[i]);
                    break;
                case 12:
                    printf("extra_lsndir:  %s\n",values[i]);
                    break;
                default:
                    printf("Err\n");
            }
        }
        printf("\n");

}

int list_backup_history(char *hisdb,PARA *para){
    sqlite3 *db;
    sqlite3_open(hisdb,&db);

    int cres = 0;
    int fres = 0;
    int pres = 0;
    int i = 0;
    my_ulonglong num_rows = 0;
    char *query = NULL;

    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);


    switch(para->argclen){
        case 1:
            print_history_help();
            break;
        case 2:
            print_history_help();
            break;
        case 3:
            if(strstr("list",para[1].content)){
                if(strstr("history",para[2].content)){
                    if(strstr("backup",para[3].content)){
                        snprintf(query,DFTLENGTH*2,"select id,metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,is_deleted,created_datetime,updated_datetime,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir from t_xtra_backup_metadata");
                        sqlite3_exec(db,query,list_backup_history_callbak,NULL,NULL);
                    }
                    else{
                        print_history_help();
                    }
                }
                else{
                    print_history_help();
                }
            }
            else{
                print_history_help();
            }

            break;
        case 4:
            print_history_help();
            break;
        case 5:
            if(strstr("list",para[1].content)){
                if(strstr("history",para[2].content)){
                    if(strstr("backup",para[3].content)){
                        if(strstr("begin",para[4].content)){
                            if(strlen(para[5].content) !=0){
                                snprintf(query,DFTLENGTH*2,"select id,metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,is_deleted,created_datetime,updated_datetime,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir from t_xtra_backup_metadata where created_datetime > '%s'",para[5].content);
                                sqlite3_exec(db,query,list_backup_history_callbak,NULL,NULL);
                            }
                            else{
                                print_history_help();
                            }
                        }
                        else{
                            print_history_help();
                        }
                    }
                    else{
                        print_history_help();
                    }
                }
                else{
                    print_history_help();
                }
            }
            else{
                print_history_help();
            }

            break;
        case 6:
            print_history_help();
            break;
        case 7:
            if(strstr("list",para[1].content)){
                if(strstr("history",para[2].content)){
                    if(strstr("backup",para[3].content)){
                        if(strstr("begin",para[4].content)){
                            if(strlen(para[5].content) !=0){
                                if(strstr("end",para[6].content)){
                                    if(strlen(para[7].content) != 0){
                                        snprintf(query,DFTLENGTH*2,"select id,metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,is_deleted,created_datetime,updated_datetime,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir from t_xtra_backup_metadata where created_datetime between  '%s' and '%s'",para[5].content,para[7].content);
                                        sqlite3_exec(db,query,list_backup_history_callbak,NULL,NULL);
                                    }
                                    else{
                                        print_history_help();
                                    }
                                }
                                else{
                                    print_history_help();
                                }
                            }
                            else{
                                print_history_help();
                            }
                        }
                        else{
                            print_history_help();
                        }
                    }
                    else{
                        print_history_help();
                    }
                }
                else{
                    print_history_help();
                }
            }
            break;
        default:
            printf("Err\n");
    }
    return(fres);
}


/***********************************************************************
 * some database operation history.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int list_restore_history(){
}


/***********************************************************************
 * some database operation history.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int list_archlog_history(){
}


/***********************************************************************
 * some database operation history.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int list_event_schedule_history(){
}

/***********************************************************************
 * pdb shell.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int pdb_shell(DBP *dbp){
    int pres = 0;
    int exeres = 0;

    static int res = 0;
    static char *connhost = NULL;
    static char *connuser = NULL;
    static char *connpass = NULL;
    static char *connsocket = NULL;
    static char *connport = NULL;

    connhost = (char *)malloc(sizeof(char)*(DFTLENGTH/4));
    connuser = (char *)malloc(sizeof(char)*(DFTLENGTH/4));
    connpass = (char *)malloc(sizeof(char)*(DFTLENGTH/2));
    connsocket = (char *)malloc(sizeof(char)*(DFTLENGTH/2));
    connport = (char *)malloc(sizeof(char)*(DFTLENGTH/4));
    memset(connhost,0,DFTLENGTH/4);
    memset(connuser,0,DFTLENGTH/4);
    memset(connpass,0,DFTLENGTH/2);
    memset(connsocket,0,DFTLENGTH/2);
    memset(connport,0,DFTLENGTH/4);

    res = parse_database_conn_params(pdb_conn_info,dbp);
    if(res ==0){
        snprintf(connhost,DFTLENGTH/4,"-h%s",dbp->host);
        snprintf(connuser,DFTLENGTH/4,"-u%s",dbp->user);
        snprintf(connpass,DFTLENGTH/2,"-p%s",dbp->pass);
        snprintf(connsocket,DFTLENGTH/2,"-S%s",dbp->socket);
        snprintf(connport,DFTLENGTH/4,"-P%d",dbp->port);
        //execute /usr/bin/mysql.
        exeres = execlp("/usr/bin/mysql","mysql",connhost,connuser,connpass,connport,NULL);
        if(exeres = -1){
            pres = 2;
        }
    }
    else{
        pres = 1;
    }
    return(pres);
}

/*
    主函数 
*/
int main(int argc,char **argv){

    int opsres = 0;
    PARA *para = NULL;

    /*
        判断参数个数是否复合要求
    */
    DBP *dbp = NULL;
    if(argc <2){
        print_main_help();
        exit(1); 
    };

    /*
     *创建hisdb
     */
    create_hisdb_objects(hisdb);

    /*
        初始化metadata数据
    */
    META *metadata = NULL;
    metadata = (META *)malloc(sizeof(META));
    memset(metadata,0,sizeof(META));

    metadata->metadata_type = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->metadata_type,0,DFTLENGTH/8);
    metadata->base_backup_directory = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->base_backup_directory,0,DFTLENGTH/8);
    metadata->backup_directory_name = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->backup_directory_name,0,DFTLENGTH/8);
    metadata->baseon_backup = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->baseon_backup,0,DFTLENGTH/8);
    metadata->extra_lsndir = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->extra_lsndir,0,DFTLENGTH/8);
    metadata->metadata_from_lsn =0;
    metadata->metadata_to_lsn = 0;
    metadata->metadata_last_lsn = 0;
    metadata->xtrabackup_compact = 0;
    metadata->is_compressed = 0;

    /*
        初始化innobackupex参数 
    */
    INNOBAK *innobak;
    innobak = (INNOBAK *)malloc(sizeof(INNOBAK));
    innobak->innobak_bin = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->encrypt = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->encrypt_key_file = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->stream = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->use_memory = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->todir = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->fromdir = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->intodir = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->hostname = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->incremental_lsn = (char *)malloc(sizeof(char)*DFTLENGTH);
    innobak->extra_lsndir = (char *)malloc(sizeof(char)*DFTLENGTH/4);

    innobak->backup_file_name = (BFN *)malloc(sizeof(BFN));
    innobak->backup_file_name->first_name = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    innobak->backup_file_name->hostname = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->backup_file_name->backup_type = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->backup_file_name->online_or_offline = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->backup_file_name->compress_or_no = (char *)malloc(sizeof(char)*DFTLENGTH/20);
    innobak->backup_file_name->encrypt_or_no = (char *)malloc(sizeof(char)*DFTLENGTH/20);
    innobak->backup_file_name->timestamp = (char *)malloc(sizeof(char)*DFTLENGTH/20);
    innobak->backup_file_name->backup_number = (char *)malloc(sizeof(char)*DFTLENGTH/20);


    memset(innobak->innobak_bin,0,DFTLENGTH/4);
    memset(innobak->encrypt,0,DFTLENGTH/4);
    memset(innobak->encrypt_key_file,0,DFTLENGTH/4);
    memset(innobak->stream,0,DFTLENGTH/4);
    memset(innobak->use_memory,0,DFTLENGTH/4);
    memset(innobak->todir,0,DFTLENGTH/2);
    memset(innobak->fromdir,0,DFTLENGTH/2);
    memset(innobak->intodir,0,DFTLENGTH/2);
    memset(innobak->hostname,0,DFTLENGTH/2);
    memset(innobak->incremental_lsn,0,DFTLENGTH);
    memset(innobak->extra_lsndir,0,DFTLENGTH/4);

    memset(innobak->backup_file_name->first_name,0,DFTLENGTH/8);
    memset(innobak->backup_file_name->hostname,0,DFTLENGTH/4);
    memset(innobak->backup_file_name->backup_type,0,DFTLENGTH/4);
    memset(innobak->backup_file_name->online_or_offline,0,DFTLENGTH/4);
    memset(innobak->backup_file_name->compress_or_no,0,DFTLENGTH/20);
    memset(innobak->backup_file_name->encrypt_or_no,0,DFTLENGTH/20);
    memset(innobak->backup_file_name->timestamp,0,DFTLENGTH/20);
    memset(innobak->backup_file_name->backup_number,0,DFTLENGTH/20);

    innobak->parallel = 0;
    innobak->throttle = 0;
    innobak->compress = 0;
    innobak->compress_threads =0;
    
    /*
        初始化数据库连接信息变量 
    */
    dbp = (DBP *)malloc(sizeof(DBP));
    dbp->host = (char *)malloc(sizeof(char)*(DFTLENGTH/8));
    dbp->user = (char *)malloc(sizeof(char)*(DFTLENGTH/8));
    dbp->pass = (char *)malloc(sizeof(char)*DFTLENGTH);
    dbp->socket = (char *)malloc(sizeof(char)*(DFTLENGTH/4));
    
    memset(dbp->host,0,DFTLENGTH/8);
    memset(dbp->user,0,DFTLENGTH/8);
    memset(dbp->pass,0,DFTLENGTH);
    memset(dbp->socket,0,DFTLENGTH/4);
    dbp->port = 0;

    para = (PARA *)malloc(sizeof(PARA)*argc);
    memset(para,0,sizeof(PARA)*argc);

    int i;
    for(i=0;i<argc;i++){
        para[i].content = (char *)malloc(sizeof(char)*128);
        memset(para[i].content,0,128);
        para[i].argclen = argc-1;
        para[i].pos = i; 
        sprintf(para[i].content,"%s",argv[i]);
    }

   
   /*
        开始解析命令行参数
   */ 
    if(strstr("backup",para[1].content)){
        opsres = backup_database(para,dbp,innobak,metadata);
        exit(opsres);
    }
    else if(strstr("restore",para[1].content)){
        restore_database(dbp,para,metadata);
    }
    else if(strstr("list",para[1].content)){
         list_backup_history(hisdb,para);
    }
    else if(strstr("shell",para[1].content)){
        print_shell_help();
        pdb_shell(dbp);
    }
    else if(strstr("help",para[1].content)){
        print_main_help();
    }
    else{
        printf("para[0].content = %s\n",para[0].content);
    }
}
