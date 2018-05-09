#include "commandes_shell.h"
#include "processus.h"
#include "string.h"

int create_process(char ** l){
  int pid;
  if (!strcmp(l[0],"ps")){
    pid = start(ps,4000, 130, "ps",l[1]);
  } else {
    printf("commande : '%s' non reconnue",l[0]);

  }
  printf("\n");


  return pid;
}

int ps(void *arg){
  (void)arg;
  printf("PID    ETAT    CMD \n");
  for (int i =0; i<NBPROC;i++){
    if (table_processus[i] != NULL){
      printf("'%d'    '%d'    '%s' \n",table_processus[i]->pid,table_processus[i]->etat,table_processus[i]->nom);
    }
  }
  return 1;
}
