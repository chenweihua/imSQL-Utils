#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct params{
    int pos;
    int length;
    char * content;
}PARA;

int main(int argc,char **argv){
    if(argc <2){
        printf("Usage: ./test backup|restore|history ...\n");
        exit(1); 
    };

    PARA *para = NULL;

    para = (PARA *)malloc(sizeof(PARA)*argc);
    memset(para,0,sizeof(PARA)*argc);

    int i;
    for(i=0;i<argc;i++){
        para[i].content = (char *)malloc(sizeof(char)*128);
        memset(para[i].content,0,128);
        para[i].length = strlen(argv[i]);
        para[i].pos = i; 
        sprintf(para[i].content,"%s",argv[i]);
    }

    
    if(strstr("backup",para[1].content)){
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
    }
    else if(strstr("restore",para[1].content)){
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
    }
    else if(strstr("history",para[1].content)){
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
        if(strstr("list",para[2].content)){
            printf("para[2].pos = %d,para[2].content = %s\n",para[2].pos,para[2].content);
            if(strstr("backup",para[3].content)){
                printf("list database backup\n");
            }
            else {
                printf("other history operation\n");
            }
        }
        else{
            printf("other history operation\n");
        }
    }
    else if(strstr("shell",para[1].content)){
        printf("para[1].pos = %d,para[1].content = %s\n",para[1].pos,para[1].content);
        system("/usr/bin/mysql -h localhost -uroot -pk7e3h8q4");
    }
    else{
        printf("para[0].content = %s\n",para[0].content);
    }
}
