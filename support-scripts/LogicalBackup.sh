#!/bin/bash

set -e
set -x

BASEDIR="/Database/Backup"
CURTIME=`date +'%Y%m%d%H%M%S'`
DESTDIR="$BASEDIR/LogicalBackup"
DUMPBIN="/usr/bin/mydumper"
MYHOST="localhost"
MYPORT=3306
MYUSER='root'
MYPASS='111111'

function LogicalBackupDb() {
	#$1参数是要备份的数据库
	if [ -d $DESTDIR/$CURTIME/$1 ];then
		$DUMPBIN -p $MYPASS -t 8 -B $1 -c -o $DESTDIR/$CURTIME/$1 -r 3000000 -l 7200
		if [ $? -eq 0 ];then
			echo "Logical Backup Success"
		else
			echo "Logical Backup Failed"
		fi
	else
		echo "Dest Directory Not Exists,Create it"
		mkdir -p $DESTDIR/$CURTIME/$1
		if [ $? -eq 0 ];then
			$DUMPBIN -p 111111 -t 8 -B $1 -c -o $DESTDIR/$CURTIME/$1 -r 3000000 -l 7200
			if [ $? -eq 0 ];then
				echo "Logical Backup Success"
			else
				echo "Logical Backup Failed"
			fi
		else
			echo "Create Failed"
		fi
	fi
}

function Main() {
	for dbname in parafiledb parauser oits_mobile karma parchdb parast top100 parastatics  paravantage mysql
	do
		LogicalBackupDb $dbname
	done
}

Main
#End
