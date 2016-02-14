#include "pdb_backup.h"

/************************************************************************
 * 判断被备份的数据库是否存在,这个函数需要完善
 * 存在返回1，否则返回0
 * 作者: Tian, Lei [tianlei@paratera.com]
 * 时间: 20151019PM1318
 ***********************************************************************/
int database_is_exists(PARA *para){
    return(1);
}

/************************************************************************
 * 备份数据库
 * 成功返回0，否则返回非0
 * 作者: Tian, Lei [tianlei@paratera.com]
 * 时间:20151019PM1318
 **********************************************************************/
int backup_database(PARA *para,DBP *dbp,INNOBAK *innobak,META *meta){
    //定义操作时间
    struct tm *ptr;
    time_t ct;
    char *timestamp_buf = NULL;

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
    char *iincremental = NULL;
    char *iincremental_lsn = NULL;
    char *ibaseon_backup = NULL;

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
    iincremental  = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    iincremental_lsn = (char *)malloc(sizeof(char)*DFTLENGTH*2);
    ibaseon_backup = (char *)malloc(sizeof(char)*DFTLENGTH/8);

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
    memset(iincremental,0,DFTLENGTH/8);
    memset(iincremental_lsn,0,DFTLENGTH*2);
    memset(ibaseon_backup,0,DFTLENGTH/8);

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

                                snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[6].content);

                                snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                /*拼接innodbbackupex 命令行*/
                                snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,para[6].content,">",para[6].content,ibackup_file_name);
                                system(innobackupex);

                                read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                xtrabackup_write_metadata_into_db(hisdb,meta);

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
                                /*得出当前时间戳*/ 
                                ct = time(NULL);
                                ptr = localtime(&ct);
                                strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s","all");
                                snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",0);
                                snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[6].content);

                                snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                xtrabackup_read_metadata_from_db(hisdb,meta);

                                snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                /*拼接innodbbackupex 命令行*/
                                snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,iincremental,iincremental_lsn,para[6].content,">",para[6].content,ibackup_file_name);
                                system(innobackupex);

                                read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                xtrabackup_write_metadata_into_db(hisdb,meta);

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
                                if(strlen(para[7].content) != 0){
                                    cres = database_is_exists(para);
                                    if(cres >0){
                                        //pdb backup db basedb full online to '/dbbackup' 
                                        /*得出当前时间戳*/ 
                                        ct = time(NULL);
                                        ptr = localtime(&ct);
                                        strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                        snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                        snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                        snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",0);
                                        snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                        snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",0);
                                        snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                        snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                        snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                        snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);

                                        snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                        snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                        /*拼接innodbbackupex 命令行*/
                                        snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,para[7].content,">",para[7].content,ibackup_file_name);
                                        system(innobackupex);

                                        read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                        xtrabackup_write_metadata_into_db(hisdb,meta);

                                    }
                                    else{
                                        printf("No %s\n",para[3].content);
                                        return(272);
                                    }
                                }else{
                                    print_backup_help();
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
                    //pdb backup db basedb incremental online to '/dbbackup'
                    else if(strstr(para[4].content,"incremental")){
                        if(strstr(para[5].content,"online")){
                            if(strstr(para[6].content,"to")){
                                if(strlen(para[7].content) != 0){
                                    cres = database_is_exists(para);
                                    if(cres >0){
                                        /*得出当前时间戳*/ 
                                        ct = time(NULL);
                                        ptr = localtime(&ct);
                                        strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                        snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                        snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                        snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                        snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                        snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",0);
                                        snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                        snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                        snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                        snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);

                                        snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                        snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                        read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                        xtrabackup_read_metadata_from_db(hisdb,meta);

                                        snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                        snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                        /*拼接innodbbackupex 命令行*/
                                        snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,istream,iparallel,iincremental,iincremental_lsn,para[7].content,">",para[7].content,ibackup_file_name);
                                        system(innobackupex);

                                        read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                        snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                        snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                        xtrabackup_write_metadata_into_db(hisdb,meta);

                                    }
                                    else{
                                        printf("No %s\n",para[3].content);
                                        return(272);
                                    }
                                }
                                else{
                                    print_backup_help();
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
                                    snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);
                                    snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                    snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);
                                    snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,para[7].content,">",para[7].content,ibackup_file_name);
                                    system(innobackupex);

                                    meta->is_compressed = 1;

                                    read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                    xtrabackup_write_metadata_into_db(hisdb,meta);

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
                                    /*得出当前时间戳*/ 
                                    ct = time(NULL);
                                    ptr = localtime(&ct);
                                    strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                    snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s","all");
                                    snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                    snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                    snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                    snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",1);
                                    snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                    snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                    snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                    snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[7].content);

                                    snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                    snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);


                                    read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                    xtrabackup_read_metadata_from_db(hisdb,meta);

                                    snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                    snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                    /*拼接innodbbackupex 命令行*/
                                    snprintf(innobackupex,DFTLENGTH*2,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,iincremental,iincremental_lsn,para[7].content,">",para[7].content,ibackup_file_name);
                                    system(innobackupex);

                                    read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                    snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                    snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);
                                    
                                    meta->is_compressed = 1;

                                    xtrabackup_write_metadata_into_db(hisdb,meta);

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
                                        cres = database_is_exists(para);
                                        if(cres >0){
                                            //pdb backup db basedb full online compress to '/dbbackup' 
                                            /*得出当前时间戳*/ 
                                            ct = time(NULL);
                                            ptr = localtime(&ct);
                                            strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                            snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                            snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                            snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",0);
                                            snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                            snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                            snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");
                                            snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[8].content);
                                            snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                            snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);
                                            snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,para[8].content,">",para[8].content,ibackup_file_name);
                                            system(innobackupex);

                                            meta->is_compressed = 1;

                                            read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                            xtrabackup_write_metadata_into_db(hisdb,meta);

                                        }
                                        else{
                                            printf("No %s\n",para[3].content);
                                            return(282);
                                        }
                                    }else{
                                        print_backup_help();
                                        return(285);

                                    }
                                }
                                else{
                                    print_backup_help();
                                    return(283);
                                }
                            }
                            else{
                                print_backup_help();
                                return(284);
                            }
                        }
                        else{
                            print_backup_help();
                            return(285);
                        }
                    }
                    else if(strstr(para[4].content,"incremental")){
                        if(strstr("online",para[5].content)){
                            if(strstr("compress",para[6].content)){
                                if(strstr("to",para[7].content)){
                                    if(strlen(para[8].content) != 0){
                                        cres = database_is_exists(para);
                                        if(cres >0){
                                            //pdb backup db basedb incremental online compress to '/dbbackup' 
                                            /*得出当前时间戳*/ 
                                            ct = time(NULL);
                                            ptr = localtime(&ct);
                                            strftime(timestamp_buf,DFTLENGTH/20,"%Y%m%d%H%M%S",ptr);
                                            snprintf(innobak->backup_file_name->first_name,DFTLENGTH/8,"%s",para[3].content);
                                            snprintf(innobak->backup_file_name->hostname,DFTLENGTH/4,"%s",innobak->hostname);
                                            snprintf(innobak->backup_file_name->backup_type,DFTLENGTH/4,"%d",1);
                                            snprintf(innobak->backup_file_name->online_or_offline,DFTLENGTH/4,"%d",1);
                                            snprintf(innobak->backup_file_name->compress_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->encrypt_or_no,DFTLENGTH/20,"%d",1);
                                            snprintf(innobak->backup_file_name->timestamp,DFTLENGTH/20,"%s",timestamp_buf);
                                            snprintf(innobak->backup_file_name->backup_number,DFTLENGTH/20,"%s","001");

                                            snprintf(meta->base_backup_directory,DFTLENGTH/2-1,"%s",para[8].content);

                                            snprintf(ibackup_file_name,DFTLENGTH,"%s.%s.%s%s%s%s.%s.%s",innobak->backup_file_name->first_name,innobak->backup_file_name->hostname,innobak->backup_file_name->backup_type,innobak->backup_file_name->online_or_offline,innobak->backup_file_name->compress_or_no,innobak->backup_file_name->encrypt_or_no,innobak->backup_file_name->timestamp,innobak->backup_file_name->backup_number);

                                            snprintf(meta->backup_directory_name,DFTLENGTH/2-1,"%s",ibackup_file_name);

                                            read_full_backup_name_from_db(hisdb,meta,ibaseon_backup);
                                            xtrabackup_read_metadata_from_db(hisdb,meta);

                                            snprintf(iincremental,DFTLENGTH/8,"%s","--incremental");
                                            snprintf(iincremental_lsn,DFTLENGTH*2,"%s%lld","--incremental-lsn=",meta->metadata_to_lsn);
                                            /*拼接innodbbackupex 命令行*/
                                            snprintf(innobackupex,DFTLENGTH*2,"%s %s %s'mysql sysadmin %s' %s %s %s %s %s %s %s %s %s %s %s %s/%s",innobak->innobak_bin,iconn,"--databases=",para[3].content,iextra_lsndir,iencrypt,iencrypt_key_file,"--compress",icompress_threads,istream,iparallel,iincremental,iincremental_lsn,para[8].content,">",para[8].content,ibackup_file_name);
                                            system(innobackupex);

                                            read_xtrabackup_checkpoint_file(innobak->extra_lsndir,meta);

                                            snprintf(meta->baseon_backup,DFTLENGTH/8,"%s",ibaseon_backup);
                                            snprintf(meta->backup_directory_name,DFTLENGTH*2,"%s",ibackup_file_name);

                                            meta->is_compressed = 1;

                                            xtrabackup_write_metadata_into_db(hisdb,meta);

                                        }
                                        else{
                                            printf("No %s\n",para[3].content);
                                            return(289);
                                        }

                                    }
                                    else{
                                        print_backup_help();
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


