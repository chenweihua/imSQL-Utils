#include "pdb.h"

/***********************************************************************
 * 通过pdb进入到MySQL的命令提示符.
 * 正确返回0，错误返回1
 * 作者: Tian, Lei [tianlei@paratera.com]
 * 时间: 20151019PM1318
 **********************************************************************/
int pdb_shell(DBP *dbp){
    int pres = 0;
    int exeres = 0;

    static int res = 0;

    //定义连接到MySQL的参数变量
    static char *connhost = NULL;
    static char *connuser = NULL;
    static char *connpass = NULL;
    static char *connsocket = NULL;
    static char *connport = NULL;
    //初始化变量
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
    //读取配置文件中的数据库连接信息
    res = parse_database_conn_params(pdb_conn_info,dbp);
    if(res ==0){
        //为变量赋值
        snprintf(connhost,DFTLENGTH/4,"-h%s",dbp->host);
        snprintf(connuser,DFTLENGTH/4,"-u%s",dbp->user);
        snprintf(connpass,DFTLENGTH/2,"-p%s",dbp->pass);
        snprintf(connsocket,DFTLENGTH/2,"-S%s",dbp->socket);
        snprintf(connport,DFTLENGTH/4,"-P%d",dbp->port);
        //进入到MySQL命令提示符
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


