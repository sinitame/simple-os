#include "stdio.h"
#include "kbd.h"


void test(void *arg)
{
  (void)arg;
  printf("Cons_read attends 4 caractères pour l'affichage.\nSi ENTER, l'affichage à lieu : on affiche les caractères deja recus. \n \n");
  while(1){
    char string[5] = {0,0,0,0,0};
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
