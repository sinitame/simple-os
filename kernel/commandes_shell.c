#include <stdbool.h>
#include "commandes_shell.h"
#include "processus.h"
#include "string.h"
#include "stdlib.h"
#include "listchainee.h"
#include "../shared/console.h"
#include "kbd.h"


extern Liste jobs;

int create_process(char ** l){
  int pid;
  if (l[0]!=NULL ){
    if (!strcmp(l[0],"ps")){
      pid = start(ps,4000, 130, "ps",l[1]);

    } else if (!strcmp(l[0],"sleep") ){
      if (l[1]!=NULL){
        pid = start(sleep,4000, 130, "sleep",l[1]);
      } else {
        return -1;
      }

    } else if (!strcmp(l[0],"jobs")){
      jobs = changeStatus(jobs);
      afficherListe(jobs);
      return -1;

    } else if (!strcmp(l[0],"clear")){
      pid = start(clear,4000, 130, "clear",l[1]);
      return -1;

    } else if (!strcmp(l[0],"help")){
      pid = start(help,4000, 130, "help",l[1]);
      return -1;

    } else if (!strcmp(l[0],"print")){
      if (l[1]!=NULL){
        pid = start(print,4000, 130, "print",l[1]);
      } else {
        return -1;
      }
    } else if (!strcmp(l[0],"echo")){
      pid = start(echo,4000, 130, "echo",l[1]);

    } else {
      printf("Commande : '%s' non reconnue \n",l[0]);
      return -1;
    }

    return pid;
  } else {
    return -1;
  }

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

int clear(void *arg) {
  (void)arg;
  efface_ecran();
  return 1;
}


int help(void *arg) {
  (void)arg;
  printf("Voici la liste des commandes disponibles :\n\n");
  printf(" * ps - affichage des processus\n");
  printf(" * sleep t - mise en pause du processus pendant t secondes\n");
  printf(" * clear - nettoyer le shell\n");
  printf(" * print - affichage d'un message a l'ecran\n");
  printf(" * header - re-affichage de l'entete du shell\n");
  printf(" * help - affichage des commandes disponibles\n");
  printf(" * echo - fait passer l'affichage en mode echo on ou echo off \n");
  printf(" * exit - sortie du shell\n");
  printf("\n");
  return 1;
}



int print(void *arg) {
  char *argc = (char *)(arg);
  while(*argc != '\0'){
    printf("%c",*argc);
    argc++;
  }
  printf("\n");
  return 1;
}

int echo(void *arg) {
  (void)arg;
  cons_echo(!b_echo);
  return 1;
}
