all:
	gcc -g -o update_metadata src/update_metadata.c `mysql_config --cflags --libs`	
	gcc -g -o read_metadata src/read_metadata.c `mysql_config --cflags --libs`	
clean:
	rm -f update_metadata read_metadata
