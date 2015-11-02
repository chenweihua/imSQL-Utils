#include "pdb.h"

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
    if(fscanf(fp,"stream = %s\n",innobak->stream ) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"compress = %s\n",innobak->compress) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"compress_threads = %d\n",innobak->compress_threads) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"parallel = %d\n",innobak->parallel) != 1){
        res = 35;
        goto end;
    }
    if(fscanf(fp,"throttle = %d\n",innobak->throttle) != 1){
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
    char *iinnobak_bin = NULL;
    char *istream = NULL;
    char *icompress = NULL;
    char *iuse_memory = NULL;
    char *icompress_threads = NULL;
    char *iparallel = NULL;
    char *ithrottle = NULL;

    query = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    iinnobak_bin = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    istream = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    icompress = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iuse_memory = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    icompress_threads= (char *)malloc(sizeof(char)*DFTLENGTH/4);
    iparallel = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    ithrottle = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    
    memset(query,0,DFTLENGTH*2);
    memset(iinnobak_bin,0,DFTLENGTH/4);
    memset(istream,0,DFTLENGTH/4);
    memset(icompress,0,DFTLENGTH/4);
    memset(iuse_memory,0,DFTLENGTH/4);
    memset(iparallel,0,DFTLENGTH/4);
    memset(ithrottle,0,DFTLENGTH/4);
    memset(icompress_threads,0,DFTLENGTH/4);

    pres = parse_database_conn_params(pdb_conn_info,dbp);

    parse_innobackupex_params(pdb_conn_info,innobak);
    snprintf(iinnobak_bin,DFTLENGTH/4,"%s",innobak->innobak_bin);
    snprintf(istream,DFTLENGTH/4,"--stream=%s",innobak->stream);
    snprintf(icompress,DFTLENGTH/4,"%s",innobak->compress);
    snprintf(icompress_threads,DFTLENGTH/4,"--compress_threads=%d",innobak->compress_threads);
    snprintf(iparallel,DFTLENGTH/4,"--parallel=%d",innobak->parallel);
    snprintf(ithrottle,DFTLENGTH/4,"--throttle=%d",innobak->throttle);
    snprintf(iuse_memory,DFTLENGTH/4,"--use_memory=%s",innobak->use_memory);

    if(strlen(para[2].content) != 0){
        snprintf(query,DFTLENGTH*2,"%s%s%s","select COUNT(*) from information_schema.SCHEMATA WHERE SCHEMA_NAME='",para[2].content,"'");
        cres = connection_pdb_server(dbp,res,&row,query);
        if(cres == 0){
            if(atoi(row[0]) == 1){
                printf("row[0]=%d\n",atoi(row[0]));
                i=system("/usr/bin/innobackupex --password=k7e3h8q4 --stream=xbstream --compress . >/root/backup/1.tar");
                printf(">>>%d\n",i);
            }else{
                printf("row[0]=%d\n",atoi(row[0]));
            }
        }
    }else{
        printf("para[2].content=%s\n",para[2].content);
    }
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
        printf("Usage: ./test backup|restore|history ...\n");
        exit(1); 
    };

    //initialize innobak instance;
    INNOBAK *innobak;
    innobak = (INNOBAK *)malloc(sizeof(INNOBAK));
    innobak->innobak_bin = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->stream = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->compress = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->use_memory = (char *)malloc(sizeof(char)*DFTLENGTH/4);

    memset(innobak->innobak_bin,0,DFTLENGTH/4);
    memset(innobak->stream,0,DFTLENGTH/4);
    memset(innobak->compress,0,DFTLENGTH/4);
    memset(innobak->use_memory,0,DFTLENGTH/4);
    innobak->compress_threads=4;
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
        para[i].length = strlen(argv[i]);
        para[i].pos = i; 
        sprintf(para[i].content,"%s",argv[i]);
    }

    
    if(strstr("backup",para[1].content)){
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
        backup_database(para,dbp,innobak);
    }
    else if(strstr("restore",para[1].content)){
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
    }
    else if(strstr("history",para[1].content)){
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
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
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
        pdb_shell(dbp);
    }
    else{
        printf("para[0].content = %s\n",para[0].content);
    }
}
