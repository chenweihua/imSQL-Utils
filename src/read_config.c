#include "read_config.h"

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
