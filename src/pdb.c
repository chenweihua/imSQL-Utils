#include "xtrabackup_checkpoint.h"

/*
    主函数 
*/
int main(int argc,char **argv){

    int opsres = 0;
    PARA *para = NULL;

    /*
        判断参数个数是否复合要求
    */
    DBP *dbp = NULL;
    if(argc <2){
        print_main_help();
        exit(1); 
    };

    /*
     *创建hisdb
     */
    create_hisdb_objects(hisdb);

    /*
        初始化metadata数据
    */
    META *metadata = NULL;
    metadata = (META *)malloc(sizeof(META));
    memset(metadata,0,sizeof(META));

    metadata->metadata_type = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->metadata_type,0,DFTLENGTH/8);
    metadata->base_backup_directory = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->base_backup_directory,0,DFTLENGTH/8);
    metadata->backup_directory_name = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->backup_directory_name,0,DFTLENGTH/8);
    metadata->baseon_backup = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->baseon_backup,0,DFTLENGTH/8);
    metadata->extra_lsndir = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    memset(metadata->extra_lsndir,0,DFTLENGTH/8);
    metadata->metadata_from_lsn =0;
    metadata->metadata_to_lsn = 0;
    metadata->metadata_last_lsn = 0;
    metadata->xtrabackup_compact = 0;
    metadata->is_compressed = 0;

    /*
        初始化innobackupex参数 
    */
    INNOBAK *innobak;
    innobak = (INNOBAK *)malloc(sizeof(INNOBAK));
    innobak->innobak_bin = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->encrypt = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->encrypt_key_file = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->stream = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->use_memory = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->todir = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->fromdir = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->intodir = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->hostname = (char *)malloc(sizeof(char)*DFTLENGTH/2);
    innobak->incremental_lsn = (char *)malloc(sizeof(char)*DFTLENGTH);
    innobak->extra_lsndir = (char *)malloc(sizeof(char)*DFTLENGTH/4);

    innobak->backup_file_name = (BFN *)malloc(sizeof(BFN));
    innobak->backup_file_name->first_name = (char *)malloc(sizeof(char)*DFTLENGTH/8);
    innobak->backup_file_name->hostname = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->backup_file_name->backup_type = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->backup_file_name->online_or_offline = (char *)malloc(sizeof(char)*DFTLENGTH/4);
    innobak->backup_file_name->compress_or_no = (char *)malloc(sizeof(char)*DFTLENGTH/20);
    innobak->backup_file_name->encrypt_or_no = (char *)malloc(sizeof(char)*DFTLENGTH/20);
    innobak->backup_file_name->timestamp = (char *)malloc(sizeof(char)*DFTLENGTH/20);
    innobak->backup_file_name->backup_number = (char *)malloc(sizeof(char)*DFTLENGTH/20);


    memset(innobak->innobak_bin,0,DFTLENGTH/4);
    memset(innobak->encrypt,0,DFTLENGTH/4);
    memset(innobak->encrypt_key_file,0,DFTLENGTH/4);
    memset(innobak->stream,0,DFTLENGTH/4);
    memset(innobak->use_memory,0,DFTLENGTH/4);
    memset(innobak->todir,0,DFTLENGTH/2);
    memset(innobak->fromdir,0,DFTLENGTH/2);
    memset(innobak->intodir,0,DFTLENGTH/2);
    memset(innobak->hostname,0,DFTLENGTH/2);
    memset(innobak->incremental_lsn,0,DFTLENGTH);
    memset(innobak->extra_lsndir,0,DFTLENGTH/4);

    memset(innobak->backup_file_name->first_name,0,DFTLENGTH/8);
    memset(innobak->backup_file_name->hostname,0,DFTLENGTH/4);
    memset(innobak->backup_file_name->backup_type,0,DFTLENGTH/4);
    memset(innobak->backup_file_name->online_or_offline,0,DFTLENGTH/4);
    memset(innobak->backup_file_name->compress_or_no,0,DFTLENGTH/20);
    memset(innobak->backup_file_name->encrypt_or_no,0,DFTLENGTH/20);
    memset(innobak->backup_file_name->timestamp,0,DFTLENGTH/20);
    memset(innobak->backup_file_name->backup_number,0,DFTLENGTH/20);

    innobak->parallel = 0;
    innobak->throttle = 0;
    innobak->compress = 0;
    innobak->compress_threads =0;

    /*
        初始化数据库连接信息变量 
    */
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

   
   /*
        开始解析命令行参数
   */ 
    if(strstr("backup",para[1].content)){
        opsres = backup_database(para,dbp,innobak,metadata);
        exit(opsres);
    }
    else if(strstr("restore",para[1].content)){
        restore_database(dbp,para,metadata);
    }
    else if(strstr("list",para[1].content)){
         list_backup_history(hisdb,para);
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
