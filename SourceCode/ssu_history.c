#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssu_function.h"
#include "ssu_shell.h"
#include "ssu_history.h"
#include "ssu_quicksort.h"

extern char *history[];
extern int history_start; // current history start point
extern int history_cur; // current num

void pr_historyUsage(void){
    printf("Usage: ssu_history [OPTION]\n");
    printf("  -c: print number of each history\n");
    printf("  -n NUMBER: print history maximum NUMBER\n");
    printf("  -r: print history in reverse order\n");
}

void add_history(char *input){
    int i;
    if(history_cur < HISTORY_MAX){
        history[history_cur] = (char *)malloc(strlen(input)+1);
        strcpy(history[history_cur], input);
        history_cur++;
    }
    else{
        free(history[(history_cur % HISTORY_MAX)]);
        history[(history_cur % HISTORY_MAX)] = (char *)malloc(strlen(input)+1);
        strcpy(history[(history_cur % HISTORY_MAX)], input);
        history_cur++;
        history_start = (int)history_cur % HISTORY_MAX;
    }

}

void c_history(int num, int is_r){
     int temp, i, j;
     int cur = 0;

     struct history c_history [HISTORY_MAX];
     
     if(history_cur > HISTORY_MAX)
          temp = HISTORY_MAX;
     else
          temp = history_cur;

     for(i=0; i<temp; i++){
          for(j=0; j<=cur; j++){
               if(j==cur){
                    c_history[j].func = history[i];
                    c_history[j].cnt = 1;
                    cur++;
                    break;
               }
               if(strcmp(history[i], c_history[j].func) == 0){
                    c_history[j].cnt++;
                    break;
               }
          }
     }

     quickSort(c_history, cur);
     
     if(num > cur)
          temp = cur;
     else
          temp = num;

     if(is_r == 0){
          for(i=0; i<temp; i++)
               printf("%s %d\n", c_history[i].func, c_history[i].cnt);
     }
     else{
          for(i=temp-1; i>=0; i--)
               printf("%s %d\n", c_history[i].func, c_history[i].cnt);
     }
}

void pr_history(int num, int is_r){
    int temp, i; 
    if(is_r == 0){
         if(history_start == 0){
              if(history_cur < HISTORY_MAX){
                   if(history_cur > num)
                       temp = num;
                   else
                       temp = history_cur;
              }
              else
                  temp = num;

              for(i = 0; i < temp; i++)
                   printf("%s\n", history[i]);
         }
         else{
             if((HISTORY_MAX - history_start) < num){
                 for(i = history_start; i < HISTORY_MAX; i++)
                      printf("%s\n", history[i]);
                 for(i=0; i < (num-(HISTORY_MAX - history_start)); i++)
                     printf("%s\n", history[i]);
             }
             else{
                 for(i = history_start; i < (history_start + num); i++)
                     printf("%s\n", history[i]);
             }
         }
    }
    // r option
    else{
        if(history_start == 0){
            if(history_cur < HISTORY_MAX){
                if(history_cur > num)
                    for(i = history_cur - 1; i >= (history_cur - num); i--)
                        printf("%s\n", history[i]);
                else
                    for(i = history_cur - 1; i >= 0; i--)
                        printf("%s\n", history[i]);
            }
            else
                for(i = HISTORY_MAX - 1; i >= (HISTORY_MAX -num); i--)
                    printf("%s\n", history[i]);
        }
        else{
            if((history_start-1) < num){
                for(i = history_start - 1; i >= 0; i--)
                    printf("%s\n", history[i]);
                for(i = HISTORY_MAX-1; i >= (HISTORY_MAX - num + history_start); i--)
                    printf("%s\n", history[i]);
            }
            else
                for(i = history_start -1; i >= (history_start - num); i--)
                    printf("%s\n", history[i]);
        }
    }
}

void ssu_history(int argc, char *argv[]){
     int is_c = 0;
     int is_r = 0;
     int num = HISTORY_MAX;
     int i;

     // no option
     if(argc < 2){
        pr_history(num, is_r);
     }
     else{
          for(i=1; i<argc; i++){
               // -c
               if(strcmp(argv[i], "-c") == 0){
                    is_c = 1;
               }
               // -n
               else if(strcmp(argv[i], "-n") == 0){
                    if(i == argc-1){
                         pr_historyUsage();
                         return;
                    }
                    
                    if(isdigit(*argv[i+1])){
                         num = atoi(argv[i+1]);
                    
                         if((num < 0) || (num > HISTORY_MAX)){
                              pr_historyUsage();
                              return;
                         }else{
                              i++;
                         }
                    }else{
                         pr_historyUsage();
                         return;
                    }
               }
               // -r
               else if(strcmp(argv[i], "-r") == 0){
                   is_r = 1; 
               }
               // -cr
               else if(strcmp(argv[i], "-cr") == 0){
                    is_c = 1;
                    is_r = 1;
               }
               // -rc
               else if(strcmp(argv[i], "-rc") == 0){
                    is_c = 1;
                    is_r = 1;
               }
               // undefined option
               else{
                    printf("unknown option %s\n", argv[i]);
                    pr_historyUsage();
                    return;
               }
          }

          if(is_c == 1){
              c_history(num, is_r);
          }
          else{
              pr_history(num, is_r);
          }
     }
}
