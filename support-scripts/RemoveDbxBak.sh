#!/bin/bash

set -e
set -x

readonly BAKDIR='/Database/Backup'
readonly PBAKDIR=$BAKDIR/"PhysicalBackup"
readonly LBAKDIR=$BAKDIR/"LogicalBackup"

function remove_database_backup_imgs() {
	if [ -d $PBAKDIR ];then
		find $PBAKDIR -type f ! -name "uncompress.sh" -mtime +3 -exec rm -f {} \;
		return $?
	else
		return 1
	fi
}

function remove_database_logical_export() {
	if [ -d $LBAKDIR ];then
		find $LBAKDIR -maxdepth 1 -type d -mtime +2 -exec rm -fr {} \;
		return $?
	else
		return 1
	fi
}

#main
remove_database_backup_imgs
remove_database_logical_export

#End.
