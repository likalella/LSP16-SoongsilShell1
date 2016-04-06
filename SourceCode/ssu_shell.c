#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ssu_function.h"
#include "ssu_shell.h"
#include "ssu_history.h"

extern struct ssu_function cmdList[];
extern char *history[];
extern int history_start;
extern int history_cur;

void pr_shellUsage(){
     printf("too many argument. plz input less than or equal to 10 argument\n");
}

void ssu_shell(void){
     int argc = 0;
     char *argv[10];
     struct timeval sTime;
     struct timeval eTime;
	 long sec;
	 long micro;
     char buf[BUFSIZE];
     char buf2[BUFSIZE];
     char temp[BUFSIZE];
     char* str;
     int i;
     
     printf("20142404 $ ");
     scanf("%[^\n]s", buf);
     gettimeofday(&sTime, NULL);

     if(*buf == '\0'){
         while(getchar() != '\n');
         return;
     }

     strcpy(buf2, buf);
     str = strtok(buf, " \t");
     argv[0] = str;
     strcpy(temp, str);

     while(1){
          argc++;
          str = strtok(NULL, " \t");
          if(str == NULL)
              break;
          strcat(temp, " ");
          strcat(temp, str);
          if(argc < 10)
              argv[argc] = str;
     }

     add_history(temp);

     if(argc > 10)
          pr_shellUsage();
     else{
          for(i=0; i<3; i++){
               if(i==2){
                    if(argv[0] != NULL){
                         system(buf2);
                    }
               }
               else if(!strcmp(cmdList[i].cmd, argv[0])){
                    cmdList[i].func(argc, argv);
                    break;
               }
          }
     }
    
     memset(buf, '\0', 1);
     memset(buf2, '\0', 1);
     memset(temp, '\0', 1);

     gettimeofday(&eTime, NULL);
	 
	 sec = (eTime.tv_sec - sTime.tv_sec);
	 micro = (eTime.tv_usec - sTime.tv_usec);

	 if(micro >= 1000000){
	 	sec += 1;
		micro -= 1000000;
	 }
	 if(micro < 0){
	 	sec -= 1;
		micro += 1000000;
	 }

	 while(micro > 999)
	 	micro /=10;

	 while(micro < 100)
        micro *= 10;

     printf("time : %ld.%ld\n", sec,micro);

     while(getchar() != '\n');
     fflush(stdout);
}
