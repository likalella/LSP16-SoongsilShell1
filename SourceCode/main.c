#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "ssu_function.h"
#include "ssu_shell.h"
#include "ssu_history.h"
#include "ssu_quicksort.h"

struct ssu_function cmdList[] = {
     {"ssu_history", ssu_history},
     {"ssu_grep", ssu_grep},
     {NULL}
};

char *history[HISTORY_MAX];
int history_start = 0;
int history_cur = 0;

// save history file.
void history_out(int signo){
     int fd, i,j, temp;
     fd = open(".ssu_history", O_RDWR | O_CREAT | O_TRUNC, 0644);
	if(history_cur < HISTORY_MAX){
	    for(i=0; i<history_cur; i++){
			strcat(history[i], "\n");
			write(fd, history[i], strlen(history[i]));
          }
	}
     else{
          for(i=(history_cur % HISTORY_MAX); i < HISTORY_MAX; i++){
		strcat(history[i], "\n");
		write(fd, history[i], strlen(history[i]));
	}
		for(i=0; i < (history_cur % HISTORY_MAX); i++){
		   strcat(history[i], "\n");
		   write(fd, history[i], strlen(history[i]));
		}
	 }
	printf("\n");
     close(fd);
     exit(0);
}

//history file read
void history_in(void){
     int fd;
	int i=0;
     char c;
	char *string;
     char n = '\0';

	string = (char*)malloc(BUFSIZE);
	memset(string, '\0', sizeof(string));
     if((fd = open(".ssu_history", O_RDONLY)) == -1)
          return;

     while(1){
          if((read(fd, &c, sizeof(c))) <= 0){
               break;
          }
          if(c =='\n'){
			  string[i] = '\0';
			  history[history_cur] = (char*)malloc(strlen(string));
			  strcpy(history[history_cur], string);
			  memset(string, '\0', sizeof(string));
			  history_cur++;
			  i=0;
          }
          else{
			  string[i] = c;
			  i++;
          }
     }
	 free(string);
}

int main(void)
{
     signal(SIGINT, (void *)history_out);

     history_in(); 
     while(1){
          ssu_shell();
     }
     return 0;
}

