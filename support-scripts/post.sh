#!/bin/bash

#定义保存配置文件的基础目录
BASEPATH='/etc/sysconfig/pdb'

function adaptive_cpus_number (){
    PCPU=`cat /proc/cpuinfo |grep "physical id"|sort|uniq|wc -l`
    #Get current machine physical cpu number.
    sed -i "s/parallel = 4/parallel = $(( $PCPU/2 ))/g" $BASEPATH/innobackupex
    sed -i "s/throttle = 4/throttle = $(( $PCPU/2 ))/g" $BASEPATH/innobackupex
}

function adaptive_hostname (){
    MNAME=`hostname`
    sed -i "s/hostname = developer-rhel6node1/hostname = $MNAME/g" $BASEPATH/innobackupex
}

function initialize_innobak_files (){
    if [ -d $BASEPATH ];then
        if [ ! -f $BASEPATH/innobackupex ];then
            /bin/cat >$BASEPATH/innobackupex <<EOF
innobak_bin = /usr/bin/innobackupex
parallel = 4
throttle = 4
use_memory = 128MB
encrypt = AES256
encrypt_key_file = /etc/sysconfig/pdb/secure.key
stream = xbstream
extra_lsndir = /tmp
hostname = developer-rhel6node1
EOF
        else
            echo "$BASEPATH/innobackupex exists.skip create it"
        fi
    else
        echo "$BASEPATH not exists,create it."
        mkdir -p $BASEPATH
        if [ ! -f $BASEPATH/innobackupex ];then
            /bin/cat >$BASEPATH/innobackupex <<EOF
innobak_bin = /usr/bin/innobackupex
parallel = 4
throttle = 4
use_memory = 128MB
encrypt = AES256
encrypt_key_file = /etc/sysconfig/pdb/secure.key
stream = xbstream
extra_lsndir = /tmp
hostname = developer-rhel6node1
EOF
        else
            echo "$BASEPATH/innobackupex exists.skip create it"
        fi
    fi
}

function initialize_db_files () {
    if [ -d $BASEPATH ];then
        if [ ! -f $BASEPATH/db.properties ];then
            /bin/cat >$BASEPATH/db.properties <<EOF
host = 127.0.0.1
user = root
socket = /var/lib/mysql/mysql.sock
pass = k7e3h8q4
port = 3306
EOF
        else
            echo "$BASEPATH/db.properties exists.skip create it"
        fi
    else
        echo "$BASEPATH not exists,create it."
        mkdir -p $BASEPATH
        if [ ! -f $BASEPATH/db.properties ];then
            /bin/cat >$BASEPATH/db.properties <<EOF
host = 127.0.0.1
user = root
socket = /var/lib/mysql/mysql.sock
pass = k7e3h8q4
port = 3306
EOF
        else
            echo "$BASEPATH/db.properties exists.skip create it"
        fi
    fi
}


function initialize_secure_file (){
    #如果加密文件不存在则创建
    if [ -f $BASEPATH/secure.key ];then
        echo "Secure.key exists,skip create it."
    else 
        echo -n `openssl rand -base64 24` >$BASEPATH/secure.key
    fi
}

function main (){
    initialize_innobak_files
    initialize_db_files
    initialize_secure_file
    adaptive_cpus_number
    adaptive_hostname
}

main
