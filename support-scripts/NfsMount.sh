#!/bin/bash

set -e
#set -x

#nfs本地挂载目录
readonly NFSLPATH='/Database/Backup/NfsBackup'
#nfs远程共享目录
readonly NFSRPATH='stage01:/stage/'

function is_mount_alive () {
	ps -ef|grep mount.nfs |grep -v 'grep' 2>&1 >/dev/null
	return $?
}

function KillMount() {
	for i in `seq 30`;do
		sleep 1
		if [ $i -eq 15 ];then
			is_mount_alive
			if [ $? -eq 0 ];then
				killall -9 mount.nfs
				if [ $? -eq 0 ];then
					echo `date +"%D %T"`":mount is killed" >>/tmp/killed.log
					return 0
				else
					return 3
				fi
			else
				return 2 
			fi
		fi
	done &	
}

function MountNfs() {
	#如果参数1为1就挂载nfs共享目录，否则卸载nfs共享目录。
	if [ $1 -eq 1 ];then
		KillMount 
		mount -t nfs $NFSRPATH $NFSLPATH
		return $?
	else
		#cd /dbbackup
		umount $NFSLPATH
		if [ $? -ne 0 ];then
			sleep 3
			umount $NFSLPATH
			if [ $? -ne 0 ];then
				umount -l $NFSLPATH
				return $?
			fi
		fi
	fi
}

MountNfs $1
