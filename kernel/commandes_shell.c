#include "commandes_shell.h"
#include "processus.h"
#include "string.h"

int create_process(char ** l){
  int pid;
  if (!strcmp(l[0],"ps")){
    pid = start(ps,4000, 128, "ps",l[1]);
  }
  return pid;
}

int ps(void *arg){
  (void)arg;
  printf("fonction ps\n");
  return 1;
}
