#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>
#include "xtrabackup_checkpoint.h"

int chk_table_exists_callback(void *,int,char **,char **);
int create_hisdb_objects(char *);
int xtrabackup_write_metadata_into_db(char *,META *);
int xtrabackup_read_metadata_from_db_callback(void *,int,char **,char **);
int xtrabackup_read_metadata_from_db(char *,META *);
int database_backup_is_exists_callback(void *,int,char **,char **);
char * database_backup_is_exists(char *,PARA *);
int read_innobackup_content_from_db_callback(void *,int,char **,char **);
int read_innobackup_content_from_db(char *,PARA *,META *);

