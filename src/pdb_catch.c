#include "pdb_catch.h"

/**************************************************************
 * 执行死锁和慢查询捕捉操作
 * 成功返回0，否则返回非0
 * 作者： Tian, Lei [tianlei@paratera.com]
 * 时间：20160222AM1024
 * **********************************************************/
int
catch_function(DBP *dbp,PARA *para){
    
    switch(para->argclen){
        case 2:
            print_catch_help();
            break;
        case 3:
            if( strcmp("deadlock",para[2].content) == 0 ){
                if( strcmp("enable",para[3].content) == 0 ){
                }
                else if( strcmp("disable",para[3].content) == 0 ){
                }
                else{
                    //如果第三个参数不是enable,也不是disable，就打印帮助信息。
                    print_catch_help();
                }
            }
            else if(strcmp("slowquery",para[2].content) == 0 ){
                if( strcmp("enable",para[3].content) == 0 ){
                }
                else if( strcmp("disable",para[3].content) == 0 ){
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
            break;
        case 5:
            break;
        default:
            printf("para->argclen >")
    }
}
