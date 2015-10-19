#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>

typedef struct metadata{
    char *    metadata_type;
    long long metadata_from_lsn;
    long long metadata_to_lsn;
    long long metadata_last_lsn;
    int       xtrabackup_compact;
    char *    base_backup_directory;
    char *    backup_directory_name;
    char *    baseon_backup;
    char *    extra_lsndir;
}META;
    
