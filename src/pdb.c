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
int backup_database(PARA *para,DBP *dbp,INNOBAK *innobak){
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
                                snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                /*拼接innodbbackupex 命令行*/
                                snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,para[6].content,">",para[6].content,ibackup_file_name);
                                system(innobackupex);
                                
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
                                cres = database_is_exists(dbp,res,&row,para);
                                if(cres >0){
                                        //pdb backup db basedb full online to '/dbbackup' 
                                        //snprintf(innobackupex,DFTLENGTH*2,"%s --password=%s    >/root/backup/2.tar",iinnobak_bin,dbp->pass,istream,icompress,icompress_threads,iparallel,ithrottle);
                                        //i=system(innobackupex);
                                        if(i == 0){
                                            printf("Backup Success!\n");
                                            return(0);
                                        }
                                        else{
                                            printf("Backup Failure!\n");
                                            return(271);
                                        }
                                    }
                                    else{
                                        printf("No %s\n",para[3].content);
                                        return(272);
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
                    else if(strstr(para[4].content,"incremental")){
                        if(strstr(para[5].content,"online")){
                            if(strstr(para[6].content,"to")){
                                snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[3].content,"'");
                                cres = connection_pdb_server(dbp,res,&row,query);
                                if(cres == 0){
                                    if(atoi(row[0]) == 1){
                                        //pdb backup db basedb incremental online to '/dbbackup' 
                                        //snprintf(innobackupex,DFTLENGTH*2,"%s --password=%s %s %s %s %s %s /root/backup  >/root/backup/2.tar",iinnobak_bin,dbp->pass,istream,icompress,icompress_threads,iparallel,ithrottle);
                                        //i=system(innobackupex);
                                        if(i == 0){
                                            printf("Backup Success!\n");
                                            return(0);
                                        }
                                        else{
                                            printf("Backup Failure!\n");
                                            return(271);
                                        }
                                    }
                                    else{
                                        printf("No %s\n",para[3].content);
                                        return(272);
                                    }
                                }
                                else{
                                    printf("connection_pdb_server failure\n");
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
                                    snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);
                                    printf("pdb backup all full online compress to /dbbackup\n");
                                    snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,para[7].content,">",para[7].content,ibackup_file_name);
                                    system(innobackupex);
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
                                    printf("pdb backup all incremental online compress to /dbbackup\n");
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
                                        snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[3].content,"'");
                                        cres = connection_pdb_server(dbp,res,&row,query);
                                        if(cres == 0){
                                            if(atoi(row[0]) == 1){
                                                //pdb backup db basedb full online compress to '/dbbackup' 
                                                //snprintf(innobackupex,DFTLENGTH*2,"%s --password=%s %s %s %s %s %s /root/backup  >/root/backup/2.tar",iinnobak_bin,dbp->pass,istream,icompress,icompress_threads,iparallel,ithrottle);
                                                //i=system(innobackupex);
                                                if(i == 0){
                                                    printf("Backup Success!\n");
                                                }
                                                else{
                                                    printf("Backup Failure!\n");
                                                    return(281);
                                                }
                                            }
                                            else{
                                                printf("No %s\n",para[3].content);
                                                return(282);
                                            }
                                        }
                                        else{
                                            printf("connection_pdb_server failure\n");
                                            return(283);
                                        }
                                    }
                                    else{
                                        printf("Backup Directory Path is must.\n");
                                        return(284);
                                    }
                                }
                                else{
                                    print_backup_help();
                                    return(285);
                                }
                            }
                            else{
                                print_backup_help();
                                return(286);
                            }
                        }
                        else{
                            print_backup_help();
                            return(287);
                        }
                    }
                    else if(strstr(para[4].content,"incremental")){
                        if(strstr("online",para[5].content)){
                            if(strstr("compress",para[6].content)){
                                if(strstr("to",para[7].content)){
                                    if(strlen(para[8].content) != 0){
                                        snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[3].content,"'");
                                        cres = connection_pdb_server(dbp,res,&row,query);
                                        if(cres == 0){
                                            if(atoi(row[0]) == 1){
                                                //pdb backup db basedb incremental online compress to '/dbbackup' 
                                                //snprintf(innobackupex,DFTLENGTH*2,"%s --password=%s %s %s %s %s %s /root/backup  >/root/backup/2.tar",iinnobak_bin,dbp->pass,istream,icompress,icompress_threads,iparallel,ithrottle);
                                                //i=system(innobackupex);
                                                if(i == 0){
                                                    printf("Backup Success!\n");
                                                }
                                                else{
                                                    printf("Backup Failure!\n");
                                                    return(288);
                                                }
                                            }
                                            else{
                                                printf("No %s\n",para[3].content);
                                                return(289);
                                            }

                                        }
                                        else{
                                            printf("connection_pdb_server failure\n");
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
                    else{

                        print_backup_help();
                        return(2815);
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
        opsres = backup_database(para,dbp,innobak);
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
