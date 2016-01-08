#include "pdb.h"
#include "sqlite3.h"

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
    return(0);
}


int list_backup_history(char *hisdb,PARA *para){
    sqlite3 *db;
    sqlite3_open(hisdb,&db);

    int cres = 0;
    int fres = 0;
    int pres = 0;
    int i = 0;
    int num_rows = 0;
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


