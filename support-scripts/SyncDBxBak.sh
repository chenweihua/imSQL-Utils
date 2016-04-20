#!/bin/bash

set -e 
set -x

readonly BASEPATH='/Database/Backup/'
readonly PSPATH="$BASEPATH/PhysicalBackup/"
readonly PDPATH="$BASEPATH/NfsBackup/PhysicalBackup/"
readonly LSPATH="$BASEPATH/LogicalBackup/"
readonly LDPATH="$BASEPATH/NfsBackup/LogicalBackup/"

function sync_database_pbackup_files() {
	local ret=0

	cd $PSPATH
	BDIR=`find . -maxdepth 1 -type d ! -name '.'|awk -F'/' '{print $2}'|head -n1`


	if [ -x /usr/bin/rsync ];then
		if [[ $BDIR != '' ]];then
			/usr/bin/rsync -avoug --progress --exclude $BDIR $PSPATH $PDPATH
		else
			/usr/bin/rsync -avoug --progress $PSPATH $PDPATH
		fi
		ret=$?
	else
		ret=1
		
	fi


	return $ret

}

function sync_database_lbackup_files() {
	local ret=0

	if [ -x /usr/bin/rsync ];then
		/usr/bin/rsync -avoug --progress $LSPATH $LDPATH
		ret=$?
	fi
	return $ret
}

function Main() {
	
	cd $BASEPATH
	#Mount Filesystem.	
	/usr/bin/NfsMount 1

	sync_database_pbackup_files
	sync_database_lbackup_files
	
	#Unmount Filesystem
	cd $BASEPATH
	/usr/bin/NfsMount 0
}

#main
Main
#End.
