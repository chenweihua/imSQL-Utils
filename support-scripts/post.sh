#!/bin/bash

BASEPATH='/etc/sysconfig/pdb'

if [ -d $BASEPATH ];then
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

    /bin/cat >$BASEPATH/db.properties <<EOF
host = 127.0.0.1
user = root
socket = /var/lib/mysql/mysql.sock
pass = k7e3h8q4
port = 3306
EOF

else
    /bin/mkdir -p $BASEPATH
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

    /bin/cat >$BASEPATH/db.properties <<EOF
host = 127.0.0.1
user = root
socket = /var/lib/mysql/mysql.sock
pass = k7e3h8q4
port = 3306
EOF

fi
echo -n `openssl rand -base64 24` >$BASEPATH/secure.key


