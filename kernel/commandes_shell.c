#include "commandes_shell.h"
#include "processus.h"
#include "string.h"
#include "stdlib.h"

int create_process(char ** l){
  int pid;
  if (!strcmp(l[0],"ps")){
    pid = start(ps,4000, 130, "ps",l[1]);
  } else if (!strcmp(l[0],"sleep")){
    pid = start(sleep,4000, 130, "sleep",l[1]);
  }
  else {
    printf("commande : '%s' non reconnue",l[0]);
  }
  printf("\n");
  return pid;
}

int ps(void *arg){
  (void)arg;
  const char *etat[] = {"actif","activable","wait_sem","wait_io","wait_child","endormi","zombie","mort"};
  printf("PID    ETAT    CMD \n");
  for (int i =0; i<NBPROC;i++){
    if (table_processus[i] != NULL){
      printf("%d    %s    %s \n",table_processus[i]->pid,etat[table_processus[i]->etat],table_processus[i]->nom);
    }
  }
  return 1;
}

int sleep(void *arg){
  char *ptr;
  wait_clock((uint32_t)strtol(arg,&ptr,10));
  return 1;
}
