#include "pdb_restore.h"

/***********************************************************************
 * restore database operation.
 *@return TRUE on success,FALSE on failure.
 * Author: Tian, Lei [tianlei@paratera.com]
 * Date:20151019PM1318
 */
int restore_database(DBP *dbp,PARA *para,META *metadata){

    //MYSQL_RES *res = NULL;
    //MYSQL_ROW row;

    char * backup_is_exists = NULL;
    backup_is_exists = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    memset(backup_is_exists,0,sizeof(char)*DFTLENGTH/2);

    int pres = 0;

    char *restore_ops = NULL;
    char *restore_applog = NULL;
    int restore_ops_res = 0;
    int applog_ops_res = 0;

    char *incremental_restore_path = NULL;
    char *remove_incremental_path_ops = NULL;
    char *decompress_ops = NULL;
    incremental_restore_path = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    remove_incremental_path_ops = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    decompress_ops = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(incremental_restore_path,0,DFTLENGTH*2);
    memset(remove_incremental_path_ops,0,DFTLENGTH*2);
    memset(decompress_ops,0,DFTLENGTH*2);

    restore_ops = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    restore_applog = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    memset(restore_ops,0,DFTLENGTH*2);
    memset(restore_applog,0,DFTLENGTH*2);

    //获取数据库参数
    pres = parse_database_conn_params(pdb_conn_info,dbp);

    switch(para->argclen){
        case 2:
            print_restore_help();
            break;
        case 3:
            print_restore_help();
            break;
        case 4:
            backup_is_exists = database_backup_is_exists(hisdb,para);
            if(strcmp(backup_is_exists,"1") == 0){
                if(strstr("into",para[3].content)){
                    if(strlen(para[4].content) != 0){
                        //pdb restore 100 into /Database
                        read_innobackup_content_from_db(hisdb,para,metadata);
                        if(strstr(metadata->metadata_type,"full-backuped")){
                            printf("full-backuped\n");
                            snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",metadata->base_backup_directory,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",para[4].content);
                            restore_ops_res = system(restore_ops);
                            if(restore_ops_res == 0){
                                if(metadata->is_compressed = 1){
                                    printf("Decompress Tablespaces\n");
                                    snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[4].content);
                                    system(decompress_ops);
                                }
                                snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[4].content);
                                applog_ops_res = system(restore_applog);
                                if(applog_ops_res == 0){
                                    printf("Apply Logs Success\n");
                                }
                                else{
                                    printf("Apply Logs Failure\n");
                                }
                            }
                            else{
                                printf("Restore Failure\n");
                            }
                        }
                        else if(strstr(metadata->metadata_type,"incremental")){
                            printf("incremental\n");
                            //restore full backup .
                            snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",metadata->base_backup_directory,metadata->baseon_backup,"/usr/bin/xbstream","-x","-C",para[4].content);
                            printf("restore full backup %s/%s\n",metadata->base_backup_directory,metadata->baseon_backup);
                            restore_ops_res = system(restore_ops);
                            if(restore_ops_res == 0){
                                //Decompress Tablespaces.
                                if(metadata->is_compressed = 1){
                                    printf("Decompress Tablespaces\n");
                                    snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[4].content);
                                    system(decompress_ops);
                                }
                                snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[4].content);
                                applog_ops_res = system(restore_applog);
                                if(applog_ops_res == 0){
                                    printf("Apply Full Backup Logs Success\n");
                                    //restore incremental backup.
                                    memset(restore_ops,0,DFTLENGTH*2);
                                    snprintf(incremental_restore_path,DFTLENGTH*2-1,"%s/%s",para[4].content,"incr");
                                    mkdir(incremental_restore_path,0777);
                                    snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",metadata->base_backup_directory,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",incremental_restore_path);
                                    printf("restore incremental backup %s/%s\n",metadata->base_backup_directory,metadata->backup_directory_name);
                                    restore_ops_res = system(restore_ops);
                                    if(restore_ops_res == 0){
                                        //Decompress Tablespaces.
                                        if(metadata->is_compressed = 1){
                                            printf("Decompress Tablespaces\n");
                                            snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",incremental_restore_path);
                                            system(decompress_ops);
                                        }
                                        snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s %s%s","/usr/bin/innobackupex","--apply-log --redo-only",para[4].content,"--incremental-dir=",incremental_restore_path);
                                        applog_ops_res = system(restore_applog);
                                        if(applog_ops_res == 0){
                                            printf("Apply Incremental Backup Logs Success\n");
                                            snprintf(remove_incremental_path_ops,DFTLENGTH*2-1,"%s %s","rm -fr",incremental_restore_path);
                                            system(remove_incremental_path_ops);
                                        }
                                        else{
                                            printf("Apply Incremental Backup Logs Failure\n");
                                        }
                                    }
                                    else{
                                        printf("Restore Failure\n");
                                    }

                                }
                                else{
                                    printf("Apply Logs Failure\n");
                                }
                            }
                            else{
                                printf("Restore Failure\n");
                            }
                        }
                        else{
                            printf("metadata_type != full-backuped or incremental\n");
                            return(41);
                        }
                    }
                    else{
                        printf("Directory Error\n");
                        print_restore_help();
                    }
                }
                else{
                    print_restore_help();
                }
            }
            else{
                printf("No backup id %d\n",para[2].content);
                print_restore_help();
            }
            break;
        case 5:
            print_restore_help();
            break;
        case 6:
            backup_is_exists = database_backup_is_exists(hisdb,para);
            if(strcmp(backup_is_exists,"1") == 0){
                if(strstr("from",para[3].content)){
                    if(strlen(para[4].content) != 0){
                        if(strstr("into",para[5].content)){
                            if(strlen(para[6].content) != 0){
                                //pdb restore 100 from /root/backup into /Database
                                read_innobackup_content_from_db(hisdb,para,metadata);
                                if(strstr(metadata->metadata_type,"full-backuped")){
                                    printf("full-backuped\n");
                                    snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",para[4].content,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",para[6].content);
                                    restore_ops_res = system(restore_ops);
                                    if(restore_ops_res == 0){
                                        //Decompress Tablespaces.
                                        if(metadata->is_compressed = 1){
                                            printf("Decompress Tablespaces\n");
                                            snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[6].content);
                                            system(decompress_ops);
                                        }
                                        snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[6].content);
                                        applog_ops_res = system(restore_applog);
                                        if(applog_ops_res == 0){
                                            printf("Apply Logs Success\n");
                                        }
                                        else{
                                            printf("Apply Logs Failure\n");
                                        }
                                    }
                                    else{
                                        printf("Restore Failure\n");
                                    }
                                }
                                else if(strstr(metadata->metadata_type,"incremental")){
                                    printf("incremental\n");
                                    //restore full backup .
                                    snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",para[4].content,metadata->baseon_backup,"/usr/bin/xbstream","-x","-C",para[6].content);
                                    printf("restore full backup %s/%s\n",para[4].content,metadata->baseon_backup);
                                    restore_ops_res = system(restore_ops);
                                    if(restore_ops_res == 0){

                                        //Decompress Tablespaces.
                                        if(metadata->is_compressed = 1){
                                            printf("Decompress Tablespaces\n");
                                            snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",para[6].content);
                                            system(decompress_ops);
                                        }
                                        snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s","/usr/bin/innobackupex","--apply-log --redo-only",para[6].content);
                                        applog_ops_res = system(restore_applog);
                                        if(applog_ops_res == 0){
                                            printf("Apply Full Backup Logs Success\n");
                                            //restore incremental backup.
                                            memset(restore_ops,0,DFTLENGTH*2);
                                            snprintf(incremental_restore_path,DFTLENGTH*2-1,"%s/%s",para[6].content,"incr");
                                            mkdir(incremental_restore_path,0777);
                                            snprintf(restore_ops,DFTLENGTH*2-1,"%s %s %s %s %s %s/%s | %s %s %s %s","/usr/bin/xbcrypt","-d","-a AES256","-f /etc/sysconfig/pdb/secure.key","-i",para[4].content,metadata->backup_directory_name,"/usr/bin/xbstream","-x","-C",incremental_restore_path);
                                            printf("restore incremental backup %s/%s\n",para[4].content,metadata->backup_directory_name);
                                            restore_ops_res = system(restore_ops);
                                            if(restore_ops_res == 0){
                                                //Decompress Tablespaces.
                                                if(metadata->is_compressed = 1){
                                                    printf("Decompress Tablespaces\n");
                                                    snprintf(decompress_ops,DFTLENGTH*2-1,"%s %s","/usr/bin/innobackupex --decompress",incremental_restore_path);
                                                    system(decompress_ops);
                                                }
                                                snprintf(restore_applog,DFTLENGTH*2-1,"%s %s %s %s%s","/usr/bin/innobackupex","--apply-log --redo-only",para[6].content,"--incremental-dir=",incremental_restore_path);
                                                applog_ops_res = system(restore_applog);
                                                if(applog_ops_res == 0){
                                                    printf("Apply Incremental Backup Logs Success\n");
                                                    snprintf(remove_incremental_path_ops,DFTLENGTH*2-1,"%s %s","rm -fr",incremental_restore_path);
                                                    system(remove_incremental_path_ops);
                                                }
                                                else{
                                                    printf("Apply Incremental Backup Logs Failure\n");
                                                }
                                            }
                                            else{
                                                printf("Restore Failure\n");
                                            }

                                        }
                                        else{
                                            printf("Apply Logs Failure\n");
                                        }
                                    }
                                    else{
                                        printf("Restore Failure\n");
                                    }
                                }
                                else{
                                    printf("metadata_type != full-backuped or incremental\n");
                                    return(41);
                                }
                            }
                            else{
                                printf("Directory Error\n");
                                print_restore_help();
                            }
                        }
                        else{
                            print_restore_help();
                        }
                    }
                    else{
                        printf("Directory Error\n");
                        print_restore_help();
                    }
                }
                else{
                    print_restore_help();
                }
            }
            else{
                printf("No backup id %d\n",para[2].content);
                print_restore_help();
            }
            break;
        default:
            printf("Err Restore\n");
    }
}


