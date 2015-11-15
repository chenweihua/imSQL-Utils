#include "pdb.h"

/***********************************************************************
 *print help information.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
*/
void print_main_help(void)
{
    char *help_msgs = "Usage: \n \
\tpdb {backup|restore|history|shell|help} \n \
\nSample: \n \
\tpdb backup {all|db} {NULL|dbname} {full|incremental} {offline|online} [compress] {to 'path'} \n \
\tpdb restore {backup_id} [from 'path'] [into 'path'] \n \
\tpdb history list [backup|restore|archive_logs|event_schedule] [begin 'timestamp'] [end 'tiemstamp'] [detail] \n \
\tpdb shell \n \
";
    printf("%s",help_msgs);
}

/***********************************************************************
 *print help information.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
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
 *print help information.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
*/
void print_restore_help(void)
{
    char *help_msgs = "Usage: \
\tpdb restore {backup_id} [from 'path'] [into 'path'] \n \
\nSample:\n \
\t1.get backup list \n \
\t\t#pdb history list backup \n \
\t\tOR \n \
\t\t#pdb history list backup begin '2015-01-01 00:00:00' end '2015-01-07 23:59:59' \n \
\t\tYou Can get the backup id number. \n \
\t2.restore database by backup id. \n \
\t\t#pdb restore 1 from '/dbbackup' into '/Database' \n \
\t\tOR \n \
\t\t#pdb restore 1 \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *print help information.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
*/
void print_history_help(void)
{
    char *help_msgs = "Usage: \
\tpdb history list [backup|restore|archive_logs|event_schedule] [begin 'timestamp'] [end 'tiemstamp'] [detail] \n \
\nSample: \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *print help information.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
*/
void print_shell_help(void)
{
    char *help_msgs = "Usage: \
\tpdb shell \n";
    printf("%s",help_msgs);
}


/***********************************************************************
 *print help messages.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
*/
int help(void)
{
    return(0);
}

/**********************************************************************
 *Read xtrabackup_checkpoints in xtrabackup directory.
 *Save metadata informations to metadata struct.
 *@Return TRUE on success.FALSE on failure.
 *Author: Tian, Lei [tianlei@paratera.com]
 *Date: 20151019PM1713
 *********************************************************************/
static my_bool
read_xtrabackup_checkpoint_file(char *extra_lsndir,META *metadata)
{
    FILE *fp = NULL;
    my_bool  r = TRUE;
    int  t;


    static char *chk = NULL;
    chk = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(chk,0,DFTLENGTH/2);

    

    //read backup_directory_name files.
    snprintf(chk,DFTLENGTH/2-1,"%s/xtrabackup_checkpoints",extra_lsndir);
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

    snprintf(metadata->extra_lsndir,DFTLENGTH/2-1,"%s",extra_lsndir);


end:
    fclose(fp);
    free(chk);

    return(r);
}






/***********************************************************************
 *parse database connection params informations,save informations into DBP struct.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
*/
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

/***********************************************************************
 * save innobackupex params informations informations into DBP struct.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
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
 * connection pdb server.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int connection_pdb_server(DBP *dbp,MYSQL_RES *res,MYSQL_ROW *row,char *query){
    //创建MySQL数据库连接符。
    MYSQL *conn = NULL;
    conn = mysql_init(NULL);
    //连接到目标数据库
    mysql_real_connect(conn,dbp->host,dbp->user,dbp->pass,NULL,dbp->port,dbp->socket,0);

    int mysql_query_res = 0;
    mysql_query_res = mysql_query(conn,query);
    if(mysql_query_res != 0){
        return(1);
    }

    res = mysql_store_result(conn);
    if(res == NULL){
        return(2);
    }

    *row = mysql_fetch_row(res);
    return(0);
}


/***********************************************************************
 *Write backup meata info into MySQL Database.
 *@return TRUE on success,FALSE on failure.
 *Author: Tian, Lei [tianlei@paratera.com]
 *Date:20151019PM1318
 ************************************************************************/
static my_bool
xtrabackup_write_metadata_into_db(DBP *dbp,MYSQL_RES *res,MYSQL_ROW row,META *metadata)
{
    int cres = 0;
    //保存查询语句的缓冲区
    char *query = NULL;
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    snprintf(query,DFTLENGTH*2-1,"INSERT INTO sysadmin.t_xtra_backup_metadata(metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir) VALUES('%s',%lld,%lld,%lld,%lld,'%s','%s','%s','%s')",metadata->metadata_type,(long long)metadata->metadata_from_lsn,(long long)metadata->metadata_to_lsn,(long long)metadata->metadata_last_lsn,(int)metadata->xtrabackup_compact,metadata->base_backup_directory,metadata->backup_directory_name,metadata->baseon_backup,metadata->extra_lsndir);

    cres = connection_pdb_server(dbp,res,&row,query);
    if(cres == 0){
        return(0);
    }
    else{
        return(1);
    }
 
}

/***********************************************************************
 *Read backup meata info from MySQL Database.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static my_bool
xtrabackup_read_metadata_from_db(DBP *dbp,MYSQL_RES *res,MYSQL_ROW row,META *metadata)
{

    int i =0;
    int tmpi = 0;
    static int cres =0;
    char *query = NULL;
    
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    snprintf(query,DFTLENGTH*2,"%s","SELECT metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir FROM sysadmin.t_xtra_backup_metadata WHERE metadata_type='full-backuped' and is_deleted = 0 ORDER BY id DESC LIMIT 1");

    cres = connection_pdb_server(dbp,res,&row,query);

    if(row != NULL){

        for(i=0;i<9;i++)
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
                    snprintf(metadata->base_backup_directory,DFTLENGTH/8,row[i]);
                case 6:
                    snprintf(metadata->backup_directory_name,DFTLENGTH/8,row[i]);
                case 7:
                    snprintf(metadata->baseon_backup,DFTLENGTH/8,row[i]);
                case 8:
                    snprintf(metadata->extra_lsndir,DFTLENGTH/8,row[i]);
                default:
                    tmpi=0;

            }
        }
    }
    else{
        return(FALSE);
    }
    return(TRUE);
}

/***********************************************************************
 *Read backup_directory_name from MySQL Database.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
static my_bool
read_full_backup_name_from_db(DBP *dbp,MYSQL_RES *res,MYSQL_ROW row,char *buf)
{

    int i =0;
    int tmpi = 0;
    static int cres =0;
    char *query = NULL;
    
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    snprintf(query,DFTLENGTH*2,"%s","SELECT metadata_type,metadata_from_lsn,metadata_to_lsn,metadata_last_lsn,xtrabackup_compact,base_backup_directory,backup_directory_name,baseon_backup,extra_lsndir FROM sysadmin.t_xtra_backup_metadata WHERE metadata_type='full-backuped' and is_deleted = 0 ORDER BY id DESC LIMIT 1");

    cres = connection_pdb_server(dbp,res,&row,query);

    if(row != NULL){
            snprintf(buf,DFTLENGTH/8,row[6]);
    }
    else{
        return(FALSE);
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
int database_is_exists(DBP *dbp,MYSQL_RES *res,MYSQL_ROW *row,PARA *para){
    int cres = 0;
    int fres = 0;
    char *query = NULL;
    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(query,0,DFTLENGTH*2);

    snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[3].content,"'");
                                
    cres = connection_pdb_server(dbp,res,row,query);
    if(cres == 0){
       fres = atoi(*row[0]);
    }
    return(fres);
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
 * innobackupex database backup.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int innobackupex_database_backup(){
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

                                xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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

                                read_full_backup_name_from_db(dbp,res,row,ibaseon_backup);
                                xtrabackup_read_metadata_from_db(dbp,res,row,meta);

                                snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                /*拼接innodbbackupex 命令行*/
                                snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,iincremental,iincremental_lsn,para[6].content,">",para[6].content,ibackup_file_name);
                                system(innobackupex);

                                read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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
                                    cres = database_is_exists(dbp,res,&row,para);
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

                                        xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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
                                    cres = database_is_exists(dbp,res,&row,para);
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

                                        read_full_backup_name_from_db(dbp,res,row,ibaseon_backup);
                                        xtrabackup_read_metadata_from_db(dbp,res,row,meta);

                                        snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                        snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                        /*拼接innodbbackupex 命令行*/
                                        snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,iincremental,iincremental_lsn,para[7].content,">",para[7].content,ibackup_file_name);
                                        system(innobackupex);

                                        read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                        snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                        snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                        xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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

                                    read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                    xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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

                                    read_full_backup_name_from_db(dbp,res,row,ibaseon_backup);
                                    xtrabackup_read_metadata_from_db(dbp,res,row,meta);

                                    snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                    snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                    /*拼接innodbbackupex 命令行*/
                                    snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,iincremental,iincremental_lsn,para[7].content,">",para[7].content,ibackup_file_name);
                                    system(innobackupex);

                                    read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                    snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                    snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                    xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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
                                        cres = database_is_exists(dbp,res,&row,para);
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

                                            read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                            xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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
                                        cres = database_is_exists(dbp,res,&row,para);
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

                                            read_full_backup_name_from_db(dbp,res,row,ibaseon_backup);
                                            xtrabackup_read_metadata_from_db(dbp,res,row,meta);

                                            snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                            snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                            /*拼接innodbbackupex 命令行*/
                                            snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,iincremental,iincremental_lsn,para[8].content,">",para[8].content,ibackup_file_name);
                                            system(innobackupex);

                                            read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                            snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                            snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                            xtrabackup_write_metadata_into_db(dbp,res,row,meta);

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
int restore_database(PARA *para){
}

/***********************************************************************
 * some database operation history.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int operate_database_history(PARA *para){
}


/***********************************************************************
 * some database operation history.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int list_backup_history(){
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

    //read pdb conn info file.
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
        print_restore_help();
    }
    else if(strstr("history",para[1].content)){
        print_history_help();
        if(strstr("list",para[2].content)){
            printf("para[2].pos = %d,para[2].content = %s\n",para[2].pos,para[2].content);
            if(strstr("backup",para[3].content)){
                printf("list database backup\n");
            }
            else {
                printf("other history operation\n");
            }
        }
        else{
            printf("other history operation\n");
        }
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
