#include "pdb.h"

/***********************************************************************
 *打印出pdb命令的总帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
***********************************************************************/
void print_main_help(void)
{
    char *help_msgs = "Usage: \n \
\tpdb {backup|restore|list|shell|help} \n \
\nSample: \n \
\tpdb backup {all|db} {NULL|dbname} {full|incremental} {offline|online} [compress] {to 'path'} \n \
\tpdb restore {backup_id} [from 'path'] [into 'path'] \n \
\tpdb list history [backup|restore|archive_logs|event_schedule] [begin 'timestamp'] [end 'tiemstamp'] [detail] \n \
\tpdb shell \n \
";
    printf("%s",help_msgs);
}

/***********************************************************************
 *打印出pdb backup备份命令的版主信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_backup_help(void)
{
    char *help_msgs = "Usage: \n \
\tpdb backup {all|db} {NULL|dbname} {full|incremental} {offline|online} [compress] {to 'path'} \n \
\nSample:\n \
\tpdb backup all full offline  to \'/dbbackup\' \n \
\tpdb backup all full offline compress to \'/dbbackup\' \n \
\tpdb backup all full online to \'/dbbackup\' \n \
\tpdb backup all full online compress to '/dbbackup' \n \
\tpdb backup all incremental online to '/dbbackup' \n \
\tpdb backup all incremental online compress to '/dbbackup' \n \
\tpdb backup db basedb full online to '/dbbackup' \n \
\tpdb backup db basedb full online compress to '/dbbackup' \n \
\tpdb backup db basedb incremental online to '/dbbackup' \n \
\tpdb backup db basedb incremental online compress to '/dbbackup' \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *打印pdb restore恢复命令的帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_restore_help(void)
{
    char *help_msgs = "Usage: \
\tpdb restore {backup_id} [from 'path'] [into 'path'] \n \
\nSample:\n \
\t1.get backup list \n \
\t\t#pdb list history backup \n \
\t\tOR \n \
\t\t#pdb list history backup begin '2015-01-01 00:00:00' end '2015-01-07 23:59:59' \n \
\t\tYou Can get the backup id number. \n \
\t2.restore database by backup id. \n \
\t\t#pdb restore 1 from '/dbbackup' into '/Database' \n \
\t\tOR \n \
\t\t#pdb restore 1 \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *打印pdb history历史命令的帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_history_help(void)
{
    char *help_msgs = "Usage: \
\tpdb list history [backup|restore|archive_logs|event_schedule] [begin 'timestamp'] [end 'tiemstamp'] [detail] \n \
\nSample: \n \
\t\t#pdb list history backup \n \
\t\tOR \n \
\t\t#pdb list history backup begin '2014-01-01 00:00:00' end '2015-12-31 23:59:59' \n \
";
    printf("%s",help_msgs);
}


/***********************************************************************
 *打印出pdb shell命令的帮助信息.
 *返回值：本函数无返回值.
 *作者:  Tian, Lei [tianlei@paratera.com]
 *时间:    20151019PM1318
*/
void print_shell_help(void)
{
    char *help_msgs = "Usage: \
\tpdb shell \n";
    printf("%s",help_msgs);
}


