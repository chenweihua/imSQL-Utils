#include "pdb.h"

/***********************************************************************
 *parse database connection params informations.
 *@return TRUE on success,FALSE on failure.
 * Author:  Tian, Lei [tianlei@paratera.com]
 * Date:    20151019PM1318
*/
int parse_database_conn_params(char *filename,DBP *dbp){
    FILE *fp = NULL;


    fp = fopen(filename,"r");
    if(fp == NULL){
        perror("fopen()");
        return(1);
    }

    if(fscanf(fp,"host = %s\n",dbp->host) != 1){
        goto end;
    }
    if(fscanf(fp,"user = %s\n",dbp->user) != 1){
        goto end;
    }
    if(fscanf(fp,"socket = %s\n",dbp->socket) != 1){
        goto end;
    }
    if(fscanf(fp,"pass = %s\n",dbp->pass) != 1){
        goto end;
    }
    if(fscanf(fp,"port = %d\n",&(dbp->port)) != 1){
        goto end;
    }

end:
    fclose(fp);
}

/***********************************************************************
 *make metadata buffer.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int backup_database(PARA *para){
}


/***********************************************************************
 *make metadata buffer.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int restore_database(PARA *para){
}

/***********************************************************************
 *make metadata buffer.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int operate_database_history(PARA *para){
}

/***********************************************************************
 *make metadata buffer.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
*/
int pdb_shell(DBP *dbp){
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
        execlp("/usr/bin/mysql","mysql",connhost,connuser,connpass,connport,NULL);
    }
    
}

//main function.
int main(int argc,char **argv){
    PARA *para = NULL;
    DBP *dbp = NULL;
    if(argc <2){
        printf("Usage: ./test backup|restore|history ...\n");
        exit(1); 
    };
    
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
