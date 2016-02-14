pdb: libhelp_message.so libread_config.so libsqlite_ops.so libxtrabackup_checkpoint.so libpdb_backup.so libpdb_restore.so libpdb_history.so libpdb_shell.so 
	gcc -g -L. -lread_config -lsqlite_ops -lpdb_backup -lpdb_restore -lpdb_history -lpdb_shell -lxtrabackup_checkpoint -lhelp_message -o pdb ./src/pdb.c

libpdb_shell.so: 
	gcc -g -fpic -shared -L. -lread_config -lsqlite_ops -lxtrabackup_checkpoint -o libpdb_shell.so ./src/pdb_shell.c
libpdb_history.so: 
	gcc -g -fpic -shared -L. -lread_config -lsqlite_ops -lxtrabackup_checkpoint -o libpdb_history.so ./src/pdb_history.c

libpdb_restore.so: 
	gcc -g -fpic -shared -L. -lread_config -lsqlite_ops -lxtrabackup_checkpoint -o libpdb_restore.so ./src/pdb_restore.c

libpdb_backup.so: 
	gcc -g -fpic -shared -L. -lsqlite_ops -lxtrabackup_checkpoint -o libpdb_backup.so ./src/pdb_backup.c

libxtrabackup_checkpoint.so: 
	gcc -g -fpic -shared -o libxtrabackup_checkpoint.so ./src/xtrabackup_checkpoint.c

libsqlite_ops.so: 
	gcc -g -fpic -shared -L. -lhelp_message -lsqlite3 -o libsqlite_ops.so ./src/sqlite_ops.c

libread_config.so: 
	gcc -g -fpic -shared -o libread_config.so ./src/read_config.c

libhelp_message.so: 
	gcc -g -fpic -shared -o libhelp_message.so ./src/help_message.c

clean:
	rm -f pdb *.so
