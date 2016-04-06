#ifndef SSU_HISTORY_H
#define SSU_HISTORY_H

#define HISTORY_MAX 100
#define COUNT_MAX 20
void add_history(char *);

struct history{
     char *func;
     int cnt;
};
#endif
