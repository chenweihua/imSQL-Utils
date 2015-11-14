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
\tpdb backup {alldbs|db} {NULL|dbname} {full|incremental} {offline|online} [compress] {to 'path'} \n \
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
\tpdb backup {alldbs|db} {NULL|dbname} {full|incremental} {offline|online} [compress] {to 'path'} \n \
\nSample:\n \
\tpdb backup alldbs full offline  to \'/dbbackup\' \n \
\tpdb backup alldbs full offline compress to \'/dbbackup\' \n \
\tpdb backup alldbs full online to \'/dbbackup\' \n \
\tpdb backup alldbs full online compress to '/dbbackup' \n \
\tpdb backup alldbs incremental online to '/dbbackup' \n \
\tpdb backup alldbs incremental online compress to '/dbbackup' \n \
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
 * backup database operation.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int backup_database(PARA *para,DBP *dbp,INNOBAK *innobak){
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;
    unsigned int i,cres,pres;
    char *query = NULL;
    char *innobackupex = NULL;

    char *iinnobak_bin = NULL;
    char *istream = NULL;
    char *icompress = NULL;
    char *iuse_memory = NULL;
    char *icompress_threads = NULL;
    char *iparallel = NULL;
    char *ithrottle = NULL;

    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    innobackupex = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    iinnobak_bin = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    istream = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    icompress = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iuse_memory = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    icompress_threads= (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iparallel = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    ithrottle = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    
    memset(query,0,DFTLENGTH*2);
    memset(innobackupex,0,DFTLENGTH*2);
    memset(iinnobak_bin,0,DFTLENGTH/4);
    memset(istream,0,DFTLENGTH/4);
    memset(icompress,0,DFTLENGTH/4);
    memset(iuse_memory,0,DFTLENGTH/4);
    memset(iparallel,0,DFTLENGTH/4);
    memset(ithrottle,0,DFTLENGTH/4);
    memset(icompress_threads,0,DFTLENGTH/4);

    pres = parse_database_conn_params(pdb_conn_info,dbp);

    parse_innobackupex_params(inno_conn_info,innobak);
    snprintf(iinnobak_bin,DFTLENGTH/4,"%s",innobak->innobak_bin);
    snprintf(iparallel,DFTLENGTH/4,"--parallel=%d",innobak->parallel);
    snprintf(ithrottle,DFTLENGTH/4,"--throttle=%d",innobak->throttle);
    snprintf(iuse_memory,DFTLENGTH/4,"--use_memory=%s",innobak->use_memory);

    switch(para->argclen){
        case 2:
            print_backup_help();
            return(2);
            break;
        case 3:
            print_backup_help();
            break;
        case 4:
            print_backup_help();
            break;
        case 5:
            print_backup_help();
            break;
        case 6:
            if((strstr(para[2].content,"database") || strstr(para[2].content,"db"))){
                if((strlen(para[3].content) != 0)){
                    if(strstr(para[4].content,"online")){
                        if(strstr(para[5].content,"full")){
                            if(strstr(para[6].content,"compress")){
                            snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[3].content,"'");
                            cres = connection_pdb_server(dbp,res,&row,query);
                            if(cres == 0){
                                if(atoi(row[0]) == 1){
                                    snprintf(innobackupex,DFTLENGTH*2,"%s --password=%s %s %s %s %s %s /root/backup  >/root/backup/2.tar",iinnobak_bin,dbp->pass,istream,icompress,icompress_threads,iparallel,ithrottle);
                                    i=system(innobackupex);
                                    if(i == 0){
                                        printf("Backup Success!\n");
                                    }
                                    else{
                                        printf("Backup Failure!\n");
                                    }
                                }
                                else{
                                    printf("No %s\n",para[3].content);
                                }
                            }
                            else{
                                printf("connection_pdb_server failure\n");
                            }
                            }
                            else if(strstr(para[6].content,"nocompress")){
                             snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[3].content,"'");
                            cres = connection_pdb_server(dbp,res,&row,query);
                            if(cres == 0){
                                if(atoi(row[0]) == 1){
                                    snprintf(innobackupex,DFTLENGTH*2,"%s --password=%s %s %s %s %s %s /root/backup  >/root/backup/2.tar",iinnobak_bin,dbp->pass,istream,icompress,icompress_threads,iparallel,ithrottle);
                                    i=system(innobackupex);
                                    if(i == 0){
                                        printf("Backup Success!\n");
                                    }
                                    else{
                                        printf("Backup Failure!\n");
                                    }
                                }
                                else{
                                    printf("No %s\n",para[3].content);
                                }
                            }
                            else{
                                printf("connection_pdb_server failure\n");
                            }       
                            }
                            else{
                                print_backup_help();
                            }
                        }
                        else if(strstr(para[5].content,"incremental")){
                            snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[3].content,"'");
                            cres = connection_pdb_server(dbp,res,&row,query);
                            if(cres == 0){
                                if(atoi(row[0]) == 1){
                                    snprintf(innobackupex,DFTLENGTH*2,"%s --password=%s %s %s %s %s %s /root/backup  >/root/backup/2.tar",iinnobak_bin,dbp->pass,istream,icompress,icompress_threads,iparallel,ithrottle);
                                    i=system(innobackupex);
                                    if(i == 0){
                                        printf("Backup Success!\n");
                                    }
                                    else{
                                        printf("Backup Failure!\n");
                                    }
                                }
                                else{
                                    printf("No %s\n",para[3].content);
                                }

                            }
                            else{
                                print_backup_help();
                            }
                        }
                        else{
                            printf("connection_pdb_server failure\n");
                        }

                    }
                    else if(strstr(para[4].content,"offline")){
                        printf("offline backup\n");
                    }
                    else{
                        print_backup_help();
                        return(41);
                    }
                } 
                else {
                    print_backup_help();
                    return(31);
                }
            }
            else if(strstr("alldbs",para[2].content)){
                //pdb backup alldbs full offline  to '/dbbackup'
                //pdb backup alldbs full online to '/dbbackup'
                if(strstr("full",para[3].content)){
                    if(strstr("offline",para[4].content)){
                        if(strstr("to",para[5].content)){
                            if(strlen(para[5].content) != 0){
                                printf("Please backup db manual\n");
                            }
                            else{
                                print_backup_help();
                                return(22);
                            }
                        }else{
                            print_backup_help();
                            return(23);
                        }
                    }
                    else if(strstr("online",para[4].content)){
                        if(strstr("to",para[5].content)){
                            if(strlen(para[6].content) != 0){
                                printf("full online backup alldbs\n");
                            }
                            else{
                                print_backup_help();
                                return(24);
                            }
                        }
                        else{
                            print_backup_help();
                            return(25);
                        }
                    }
                    else{
                        print_backup_help();
                        return(26);
                    }
                }else if(strstr("incremental",para[3].content)){
                    //pdb backup alldbs incremental online to '/dbbackup'
                    if(strstr("online",para[4].content)){
                        if(strstr("to",para[5].content)){
                            if(strlen(para[6].content) != 0){
                                printf("onlie incremental backup alldbs\n");
                            }
                            else{
                                print_backup_help();
                                return(27);
                            }
                        }
                        else{
                            print_backup_help();
                            return(28);
                        }
                    }else{
                        print_backup_help();
                        return(29);
                    }
                }else{
                    print_backup_help();
                    return(21);
                } 
            }
            else{
                print_backup_help();
                return(20);
            }
            break;
        case 7:
            break;
        case 8:
            break;
        default:
            print_backup_help();
            return(7);
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

//main function.
int main(int argc,char **argv){
    PARA *para = NULL;
    DBP *dbp = NULL;
    if(argc <2){
        print_main_help();
        exit(1); 
    };

    //initialize innobak instance;
    INNOBAK *innobak;
    innobak = (INNOBAK *)malloc(sizeof(INNOBAK));
    innobak->innobak_bin = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->use_memory = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->todir = (char *)malloc(sizeof(char)*DFTLENGTH/2);

    memset(innobak->innobak_bin,0,DFTLENGTH/4);
    memset(innobak->use_memory,0,DFTLENGTH/4);
    memset(innobak->todir,0,DFTLENGTH/2);
    innobak->parallel = 4;
    innobak->throttle = 500;
    
    //initialize dbp instance.
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

    
    if(strstr("backup",para[1].content)){
        backup_database(para,dbp,innobak);
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
