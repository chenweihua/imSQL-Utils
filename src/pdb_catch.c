#include "pdb_catch.h"




/**************************************************************
 * 执行死锁和慢查询捕捉操作
 * 成功返回0，否则返回非0
 * 作者： Tian, Lei [tianlei@paratera.com]
 * 时间：20160222AM1024
 * **********************************************************/
int
catch_function(DBP *dbp,PARA *para){

    //定义字符缓冲池
    char *query_buf = NULL;
    query_buf = (char *)malloc(sizeof(char)*DFTLENGTH);
    memset(query_buf,0,DFTLENGTH);

    //MySQL连接符
    MYSQL *conn = NULL;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int mysql_query_ret = 0;

    //连接到MySQL
    conn = mysql_init(NULL);
    mysql_real_connect(conn,dbp->host,dbp->user,dbp->pass,NULL,dbp->port,dbp->socket,0);

    switch(para->argclen){
        case 2:
            print_catch_help();
            break;
        case 3:
            //pdb catch deadlock|slowquery enable|disable
            if( strcmp("deadlock",para[2].content) == 0 ){
                if( strcmp("enable",para[3].content) == 0 ){
                    //开启死锁监控功能
                    mysql_query_ret = mysql_query(conn,"set global innodb_print_all_deadlocks = ON");
                    return(mysql_query_ret);
                }
                else if( strcmp("disable",para[3].content) == 0 ){
                    //关闭死锁监控功能
                    mysql_query_ret = mysql_query(conn,"set global innodb_print_all_deadlocks = OFF");
                    return(mysql_query_ret);
                }
                else{
                    //如果第三个参数不是enable,也不是disable，就打印帮助信息。
                    print_catch_help();
                }
            }
            else if(strcmp("slowquery",para[2].content) == 0 ){
                if( strcmp("enable",para[3].content) == 0 ){
                    //开启慢查询监控功能
                    mysql_query_ret = mysql_query(conn,"set global slow_query_log = ON");
                    return(mysql_query_ret);
                }
                else if( strcmp("disable",para[3].content) == 0 ){
                    //关闭慢查询监控功能
                    mysql_query_ret = mysql_query(conn,"set global slow_query_log = OFF");
                    return(mysql_query_ret);
                }
                else{
                    //如果第三个参数不是enable,也不是disable，就打印帮助信息。
                    print_catch_help();
                }
            }
            else{
                //如果第二个参数既不是deadlock也不是slowquery，就打印帮助信息
                print_catch_help();
            }
            break;
        case 4:
            //pdb catch deadlock|slowquery enable|disable
            if( strcmp("deadlock",para[2].content) == 0 ){
                if( strcmp("to",para[3].content) == 0 ){
                    if( strlen(para[4].content) >0 ) {
                        //开启死锁监控功能
                        sprintf(query_buf,"%s%s%s","set global deadlock_log_path = '",para[4].content,"'");
                        mysql_query_ret = mysql_query(conn,query_buf);
                        return (mysql_query_ret);
                    }
                    else{
                        print_catch_help();
                    }
                }
                else{
                    //如果第三个参数不是enable,也不是disable，就打印帮助信息。
                    print_catch_help();
                }
            }
            else if(strcmp("slowquery",para[2].content) == 0 ){
                if( strcmp("to",para[3].content) == 0 ){
                    if( strlen(para[4].content) >0 ) {
                        //开启死锁监控功能
                        sprintf(query_buf,"%s%s/SlowQuery.log%s","set global slow_query_log_file = '",para[4].content,"'");
                        mysql_query_ret = mysql_query(conn,query_buf);
                        return (mysql_query_ret);
                    }
                    else{
                        print_catch_help();
                    }
                }
                else{
                    //如果第三个参数不是enable,也不是disable，就打印帮助信息。
                    print_catch_help();
                }
            }
            else{
                //如果第二个参数既不是deadlock也不是slowquery，就打印帮助信息
                print_catch_help();
            }
            break;
        default:
            print_catch_help();
    }
}
