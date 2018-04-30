#include "stdio.h"
#include "kbd.h"


void test(void *arg)
{
  (void)arg;
  char string[5];
  while(1){
    switch(cons_read(string,4 )){
      case 1 :
        printf("1 : %s \n" , string);
        break;
      case 2 :
        printf("2 : %s \n" , string);
        break;
      case 3 :
        printf("3 : %s \n" , string);
        break;
      case 4 :
        printf("4 : %s \n" , string);
        break;
    }
  }
}
