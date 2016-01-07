#!/bin/bash

set -e 
#set -x

readonly DBPASS=111111

function send_mail() {
	echo $@ |mail -s "$(echo -e "OITS新用户注册通知邮件 \nContent-Type: text/html;charset=utf8")" tianlei@paratera.com chenzhong@paratera.com yangkun@paratera.com
}

function sql_query() {
        #执行mysql -h xxx -u xxx -pxxx -S xxx -e xxx
        local QUERY_STMT=$@
        local OUT=`mysql -p$DBPASS -e"$QUERY_STMT" 2>/dev/null|grep -v '|'|grep -v 'email'|grep -v 'Logging'`
	echo $OUT
}

function check_new_user() {
        #拼接DDL语句
        local STMT="select '<b>用户名:</b> ',t3.email,' <b>创建时间：</b> ',t3.created_date,' <b>更新时间：</b> ',t3.updated_date,' <b>关联集群：</b> ',t3.clusterName,'</br>' from (select t1.email,t1.created_date,t1.updated_date,t2.clusterName from (select tu.email,tu.created_date,tu.updated_date  from parauser.t_user as tu  left join sysadmin.t_user_list as tul  on tul.email = tu.email  where tul.email is NULL) as t1 left join (select distinct t4.email,t4.clusterName  from  ( select distinct t3.email,tct.clusterName  from  ( select tu.user_id,tu.email,tcut.clusterId  from parauser.t_user as tu  left join parafiledb.t_clusteruser_tab as tcut  on tu.user_id = tcut.userId ) as t3  left join parafiledb.t_cluster_tab as tct  on t3.clusterId = tct.id ) as t4) as t2 on t1.email = t2.email) as t3" 
        #local STMT="select concat('<html><body>','用户名: ',t1.email,'  用户账户创建时间: ',t1.created_date,'  用户账户更新时间: ',t1.updated_date,' 用户 关联集群:',t2.clusterName,'<br></body></html>') from (select tu.email,tu.created_date,tu.updated_date from parauser.t_user as tu left join sysadmin.t_user_list as tul on tul.email = tu.email where tul.email is NULL) as t1 left join (select tut.user_name,tct.clusterName from parafiledb.t_cluster_tab as tct,parafiledb.t_user_tab as tut,parafiledb.t_clusteruser_tab as tcut where tct.id = tcut.clusterId and tut.id = tcut.userId) as t2 on t1.email = t2.user_name where t2.user_name is not null" 

	local STR1=$(sql_query $STMT)
	if [ "$STR1" != '' ];then
		#执行DML语句
        	send_mail $(sql_query $STMT)

		#把新注册用户加入到用户列表。
		local STMT2="select tu.email from parauser.t_user as tu left join sysadmin.t_user_list as tul on tul.email = tu.email where tul.email is NULL"
		local NUSR=`echo $(sql_query $STMT2)`
		for user in $NUSR;do
			local STMT3="insert into sysadmin.t_user_list(email) values('"$user"')"
			sql_query $STMT3
		done
	fi
}

function main() {
	check_new_user
}

main

#End

