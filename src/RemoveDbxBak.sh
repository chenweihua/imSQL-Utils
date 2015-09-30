#!/bin/bash

set -e
#set -x

readonly BAKDIR='/Database/Backup'
readonly PBAKDIR=$BAKDIR/"PhysicalBackup"
readonly LBAKDIR=$BAKDIR/"LogicalBackup"

function remove_database_backup_imgs() {
	if [ -d $PBAKDIR ];then
		find $PBAKDIR -type f ! -name "uncompress.sh" -mtime +7 -exec rm -f {} \;
		return $?
	else
		return 1
	fi
}

#main
remove_database_backup_imgs

#End.
