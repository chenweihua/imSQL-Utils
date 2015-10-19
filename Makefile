all:
	gcc -g -o update_metadata update_metadata.c `mysql_config --cflags --libs`	
	gcc -g -o read_metadata read_metadata.c `mysql_config --cflags --libs`	
