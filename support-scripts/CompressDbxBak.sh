#!/bin/bash

set -e
set -x

readonly BAKDIR='/Database/Backup'
readonly PBAKPATH="$BAKDIR/PhysicalBackup/"
readonly TIMES=`date +'%Y-%m-%d %H:%M:%S'`
readonly TIMED=`date +'%Y-%m-%d'`

function compress_database_backup() {
	for baks in `ls $PBAKPATH`
	do
		cd $PBAKPATH
		
		local dirtime=`stat -c %z $baks|awk '{print $1}'`
		if [ -d $baks ];then
			if [ $TIMED != $dirtime ];then
				/bin/tar -cf - $baks|pigz -p8|openssl des3 -salt -k 13901320764|dd of=$PBAKPATH/$baks.img bs=20M
				if [ $? -eq 0 ];then
                    if [ ! -d $BAKDIR/CompressLogs ];then
                        mkdir -p $BAKDIR/CompressLogs
                    fi
					echo "$TIMES -> $baks" >>$BAKDIR/CompressLogs/Compress.log
					rm -fr $baks
					if [ -x /usr/bin/md5sum ];then
						/usr/bin/md5sum $PBAKPATH/$baks.img >$PBAKPATH/$baks.md5sum
					fi
				fi
			fi
		fi
		cd $BAKDIR
	done

}

#main
compress_database_backup

#end
