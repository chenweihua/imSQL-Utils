#!/bin/bash

set -e 
set -x

readonly DEFAULTS_FILE='/etc/my.cnf'
readonly DBHOST='localhost'
readonly DBUSER='root'
readonly DBPASS='k7e3h8q4'
readonly DBSOCK='/var/lib/mysql/mysql.sock'
readonly BKBDIR='/Database/Backup/PhysicalBackup/'
readonly BKLDIR='/Database/Backup/BackupLogs/'
readonly CUR_DATE=`date +'%Y%m%d%H%M%S'`
readonly DAYOFWEEK=`date +'%u'`
readonly REGISTER='/usr/bin/registe_metadata'

function sql_query() {
        #执行mysql -h xxx -u xxx -pxxx -S xxx -e xxx
        local QUERY_STMT=$@
        mysql -p$DBPASS -e"$QUERY_STMT" 2>/dev/null
        return $?
}

function delete_old_binlog() {
        #过滤出最新备份的binlog文件名称
        local LASTBINLOG=`cat $1/xtrabackup_binlog_info |awk '{print $1}'`
        #拼接DDL语句
        local STMT="PURGE BINARY LOGS TO '"$LASTBINLOG"'"
        #执行DDL语句
        sql_query $STMT
        return $?
}


function dbbackup () {
	if [ $DAYOFWEEK -ne 8 ];then
		innobackupex --defaults-file=$DEFAULTS_FILE --host $DBHOST --user $DBUSER --password $DBPASS --socket $DBSOCK --parallel=4 --throttle=400 --databases="mysql parafiledb karma parchdb parauser oits_mobile" $BKBDIR 2>$BKLDIR/"$CUR_DATE"_FUL.log 
		if [ $? -eq 0 ];then
			echo "$CUR_DATE"_FUL.log >$BKLDIR/fullbaklist.log
			local FBAKPATH=`cat $BKLDIR/"$CUR_DATE"_FUL.log|grep "Created backup directory"|awk '{print $5}'`
            local BKRDIR=`echo $FBAKPATH|awk -F'/' '{print $NF}'`
			#delete archlog.
			delete_old_binlog $FBAKPATH
            #registe metadata into database.
            $REGISTER $BKBDIR $BKRDIR "N"
		else
			return 1
		fi
		
	else 
		FULBKF=`cat $BKLDIR/fullbaklist.log`
		FULBKDIR=`cat $BKLDIR/$FULBKF|tail -n 4|head -n 1|awk -F'/' '{print $4}'|awk -F"'" '{print $1}'`
		if [ -n $FULBKDIR ];then
			innobackupex --defaults-file=$DEFAULTS_FILE --host $DBHOST --user $DBUSER --password $DBPASS --socket $DBSOCK --parallel=4 --throttle=400 --databases="mysql parafiledb parchdb karma parauser oits_mobile" --incremental $BKBDIR --incremental-basedir=$BKBDIR/$FULBKDIR 2>$BKLDIR/"$CUR_DATE"_ICR.log
			if [ $? -eq 0 ];then
				echo "$FULBKF -> $CUR_DATE"_ICR.log >>$BKLDIR/incrbaklist.log
			else
				return 2
			fi
		fi
	fi
	return 0
}


#main
dbbackup
if [ $? -eq 1 ];then
	echo "Database Full Backup Failed!!!"
elif [ $? -eq 2 ];then
	echo "Database Incremental Backup Failed!!!"
else
	echo "Database Backup Success!!!"
fi
