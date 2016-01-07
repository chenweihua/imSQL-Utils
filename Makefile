all:
	gcc -g -o registe_metadata src/registe_metadata.c src/metadata.h `mysql_config --cflags --libs`	
	gcc -g -o read_metadata src/read_metadata.c src/metadata.h `mysql_config --cflags --libs`	
	gcc -lsqlite3 -g -o pdb	src/pdb.c src/pdb.h  `mysql_config --cflags --libs`
clean:
	rm -f registe_metadata read_metadata pdb
