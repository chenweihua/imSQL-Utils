#!/bin/bash

set -e 
set -x

readonly BASEPATH='/Database/Backup/'
readonly SPATH="$BASEPATH/PhysicalBackup/"
readonly DPATH="$BASEPATH/NfsBackup/PhysicalBackup/"

function sync_database_backup_files() {
	local ret=0

	cd $SPATH
	BDIR=`find . -maxdepth 1 -type d ! -name '.'|awk -F'/' '{print $2}'|head -n1`

	cd $BASEPATH
	
	#Mount Filesystem.	
	/opt/pdb/bin/NfsMount 1

	if [ -x /usr/bin/rsync ];then
		if [ -n $BDIR ];then
			/usr/bin/rsync -avoug --progress --exclude $BDIR $SPATH $DPATH
		fi
		ret=$?
	else
		ret=1
		
	fi

	#Unmount Filesystem
	/opt/pdb/bin/NfsMount 0

	return $ret

}

#main
sync_database_backup_files

#End.
