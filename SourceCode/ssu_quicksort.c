#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssu_history.h"

void quickSort(struct history[], int);
void qSort(struct history[], int, int);

void quickSort(struct history c_history[], int size){
     qSort(c_history, 0, size-1);
}

void qSort(struct history c_history[], int left, int right){
     int l, r;
     int p;
     struct history pivot;
     l = left;
     r = right;
     pivot.func = c_history[left].func;
     pivot.cnt = c_history[left].cnt;

     while(left < right){
          while((c_history[right].cnt <= pivot.cnt) && (left < right))
               right--;

          if(left != right){
               c_history[left].func = c_history[right].func;
               c_history[left].cnt = c_history[right].cnt;
          }

          while((c_history[left].cnt >= pivot.cnt) && (left < right))
               left++;

          if(left != right){
               c_history[right].func = c_history[left].func;
               c_history[right].cnt = c_history[left].cnt;
               right --;
          }
     }

     c_history[left].func = pivot.func;
     c_history[left].cnt = pivot.cnt;
     p = left;
     left = l;
     right = r;

     if(left < p)
          qSort(c_history, left, p-1);
     if(right > p)
          qSort(c_history, p+1, right);
}
