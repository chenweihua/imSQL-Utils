#include "xtrabackup_checkpoint.h"

//读取mysql头文件
#include <my_global.h>
#include <mysql.h>

int catch_function(DBP *,PARA *);
