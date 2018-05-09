#include <stdio.h>
#include <string.h>
#include "message_queue.h"
#include "mem.h"
#include "processus.h"

int nb_queues=0;
Message_queue *tab_message_queues[NBQUEUE]={NULL};
int enable_delete_reset[NBQUEUE]={0};

int pcount(int fid, int *count) {
  //si fid est invalide
  if (fid>=NBQUEUE || fid<0) {
    return -1;
  }
  //si la file d'identifiant fid n'était pas créée
  else if (tab_message_queues[fid]==NULL) {
    return -1;
  }
  if (count!=NULL) {
    //si la file est vide
    if (queue_empty(&tab_message_queues[fid]->messages)!=0) {
      *count = -tab_message_queues[fid]->nb_b_c;
    }
    else {
      *count = tab_message_queues[fid]->nb_msg + tab_message_queues[fid]->nb_b_p;
    }
  }
  return 0;
}

int pcreate(int count) {
  //s'il reste plus de files de messages ou le nombre count est négative ou égale à zéro
  if (nb_queues>=NBQUEUE || count<=0 || count>1000) {
    return -1;
  }

  int fid=0;
  int available_queue=0;
  //Chercher une file non utilisée
  while (fid<NBQUEUE && !available_queue) {
    if (tab_message_queues[fid]==NULL) {
      available_queue = 1;
    }
    else {
      fid++;
    }
  }
  tab_message_queues[fid] = mem_alloc(sizeof(Message_queue));
  tab_message_queues[fid]->nb_msg=0;
  tab_message_queues[fid]->nb_b_c=0;
  tab_message_queues[fid]->nb_b_p=0;
  //initialiser le nombre de messages à count
  tab_message_queues[fid]->queue_capacity=count;
  //initialiser la file des processus producteurs
  INIT_LIST_HEAD(&(tab_message_queues[fid]->blocked_producers));
  //initialiser la file des processus consomatteurs
  INIT_LIST_HEAD(&(tab_message_queues[fid]->blocked_consumers));
  //initialiser la file des messages
  INIT_LIST_HEAD(&(tab_message_queues[fid]->messages));
  //Incrémenter le nombre de files de messages utilisés
  nb_queues++;
  enable_delete_reset[fid]=0;
  return fid;
}

int pdelete(int fid) {
  if (fid>NBQUEUE || fid<0) {
    return -1;
  }
  //si la file d'identifiant fid n'était pas créée
  else if (tab_message_queues[fid]==NULL) {
    return -1;
  }
  File_priorite *proc;
  int pid_proc_c=0, pid_proc_p=0;
  //supprimer les messages
  while (tab_message_queues[fid]->nb_msg>0) {
    queue_out(&tab_message_queues[fid]->messages, File_priorite, chaine);
    tab_message_queues[fid]->nb_msg--;
  }
  //S'il existe des consomatteurs bloqués
  if (tab_message_queues[fid]->nb_b_c!=0) {
    proc=queue_top(&tab_message_queues[fid]->blocked_consumers, File_priorite, chaine);
    pid_proc_c=proc->val;
    //Supprimer les processus consommateurs
    while (tab_message_queues[fid]->nb_b_c>0) {
      proc=queue_out(&tab_message_queues[fid]->blocked_consumers, File_priorite, chaine);
      enable_delete_reset[fid]++;
      if (table_processus[proc->val]!=NULL) {
	table_processus[proc->val]->etat=activable;
      }
      tab_message_queues[fid]->nb_b_c--;
    }
  }
  //S'il existe des producteuts bloqués
  if (tab_message_queues[fid]->nb_b_p!=0) {
    proc=queue_top(&tab_message_queues[fid]->blocked_producers, File_priorite, chaine);
    pid_proc_p=proc->val;
    //Supprimer les processus producteurs
    while (tab_message_queues[fid]->nb_b_p>0) {
      File_priorite *proc;
      proc=queue_out(&tab_message_queues[fid]->blocked_producers, File_priorite, chaine);
      enable_delete_reset[fid]++;
      if (table_processus[proc->val]!=NULL) {
	table_processus[proc->val]->etat=activable;
      }
      tab_message_queues[fid]->nb_b_p--;
    }
  }
  mem_free(tab_message_queues[fid], sizeof(Message_queue));
  tab_message_queues[fid]=NULL;
  nb_queues--;
  if (pid_proc_c>processus_actif->pid || pid_proc_p>processus_actif->pid) {
    ordonnancement();
  }
  return 0;
}

int preceive(int fid,int *message) {
  if (fid>=NBQUEUE || fid<0) {
    return -1;
  }
  //si la file d'identifiant fid n'était pas créée
  else if (tab_message_queues[fid]==NULL) {
    return -1;
  }
  //s'il n'y a pas de messages dans la file
  if (queue_empty(&tab_message_queues[fid]->messages)) {
    // On enregistre le pid de ce processus dans la file des consommateurs
    File_priorite *nv_c;
    nv_c=mem_alloc(sizeof(File_priorite));
    nv_c->val=getpid();
    nv_c->prio=getprio(getpid());
    //Ajout de ce processus dans la liste des consommateurs bloqués
    queue_add(nv_c, &(tab_message_queues[fid]->blocked_consumers), File_priorite, chaine, prio);
    //Incrément du nombre des processus consomatteurs bloqués
    tab_message_queues[fid]->nb_b_c++;
    //bloquer ce processus
    table_processus[getpid()]->etat=wait_io;
    //passer la main au prochain processus
    ordonnancement();
    if (enable_delete_reset[fid]>0) {
      enable_delete_reset[fid]--;
      return -2;
    }
  }
  //s'il existe au moins un message dans la file
  //On extrait le message le plus ancien dans la file et on le place dans "message"
  File_priorite *msg;
  msg=queue_bottom(&tab_message_queues[fid]->messages, File_priorite, chaine);
  queue_del(msg, chaine);
  if (message!=NULL) {
    *message=msg->val;
    // printf("message recu : %d\n", msg->val);
  }
  mem_free(msg, sizeof(File_priorite));
  //On décrément le nombre de messages dans la file
  tab_message_queues[fid]->nb_msg--;
  update_list(fid, &tab_message_queues[fid]->blocked_producers, 1);
  //s'il existe des processus producteurs bloqués
  if (!queue_empty(&tab_message_queues[fid]->blocked_producers)) {
    // printf("liste des producteus blockés\n");
    print_list(&tab_message_queues[fid]->blocked_producers);
    //On récupére le processus bloqué le plus prioritaire
    File_priorite *proc;
    proc=queue_out(&tab_message_queues[fid]->blocked_producers, File_priorite, chaine);
    //On décrément le nombre de processus producteurs bloqués
    tab_message_queues[fid]->nb_b_p--;
    if (table_processus[proc->val]!=NULL) {
      table_processus[proc->val]->etat=activable;
    }
    ordonnancement();
  }
  return 0;
}

int preset(int fid) {
  if (fid>NBQUEUE || fid<0) {
    return -1;
  }
  //si la file d'identifiant fid n'était pas créée
  else if (tab_message_queues[fid]==NULL) {
    return -1;
  }
  File_priorite *proc;
  int pid_proc_p=0, pid_proc_c=0;
  //supprimer les messages
  while (tab_message_queues[fid]->nb_msg>0) {
    queue_out(&tab_message_queues[fid]->messages, File_priorite, chaine);
    tab_message_queues[fid]->nb_msg--;
  }
  //S'il existe des consomatteurs bloqués
  if (tab_message_queues[fid]->nb_b_c!=0) {
    proc=queue_top(&tab_message_queues[fid]->blocked_consumers, File_priorite, chaine);
    pid_proc_c=proc->val;
    //Supprimer les processus consommateurs
    while (tab_message_queues[fid]->nb_b_c>0) {
      proc=queue_out(&tab_message_queues[fid]->blocked_consumers, File_priorite, chaine);
      enable_delete_reset[fid]++;
      if (table_processus[proc->val]!=NULL) {
	table_processus[proc->val]->etat=activable;
      }
      tab_message_queues[fid]->nb_b_c--;
    }
  }
  //S'il existe des producteuts bloqués
  if (tab_message_queues[fid]->nb_b_p!=0) {
    proc=queue_top(&tab_message_queues[fid]->blocked_producers, File_priorite, chaine);
    pid_proc_p=proc->val;
    //Supprimer les processus producteurs
    while (tab_message_queues[fid]->nb_b_p>0) {
      File_priorite *proc;
      proc=queue_out(&tab_message_queues[fid]->blocked_producers, File_priorite, chaine);
      enable_delete_reset[fid]++;
      if (table_processus[proc->val]!=NULL) {
	table_processus[proc->val]->etat=activable;
      }
      tab_message_queues[fid]->nb_b_p--;
    }
  }
  if (pid_proc_c>processus_actif->pid || pid_proc_p>processus_actif->pid) {
    ordonnancement();
  }
  return 0;
}

int psend(int fid, int message){
  if (fid>=NBQUEUE || fid<0) {
    return -1;
  }
  //si la file d'identifiant fid n'était pas créée
  else if (tab_message_queues[fid]==NULL) {
    return -1;
  }
  // Si la file de message est pleine
  if (tab_message_queues[fid] -> nb_msg >= tab_message_queues[fid]-> queue_capacity){
    // On enregistre le pid de ce processus
    tab_message_queues[fid]->nb_b_p++;
    File_priorite *nv_c;
    nv_c=mem_alloc(sizeof(File_priorite));
    nv_c->val=getpid();
    nv_c->prio=getprio(getpid());
    //Ajout de ce processus dans la liste des producteurs bloqués
    queue_add(nv_c, &(tab_message_queues[fid]->blocked_producers), File_priorite, chaine, prio);
    // On met le processus en attente
    table_processus[getpid()]->etat=wait_io;
    ordonnancement();
    if (enable_delete_reset[fid]>0) {
      enable_delete_reset[fid]--;
      return -2;
    }
  }
  // Si la file de messages n'est pas pleine
  File_priorite *nv_msg;
  nv_msg=mem_alloc(sizeof(File_priorite));
  nv_msg->val=message;
  // printf("message ecrit : %d \n", nv_msg->val);
  if (queue_empty(&tab_message_queues[fid]->messages)!=0) {
    nv_msg->prio=0;
  }
  else {
    File_priorite *msg;
    msg=queue_top(&tab_message_queues[fid]->messages, File_priorite, chaine);
    nv_msg->prio=msg->prio+1;
  }
  queue_add(nv_msg, &(tab_message_queues[fid]->messages), File_priorite, chaine, prio);
  //incrément du nombre de messages dans la file
  tab_message_queues[fid]->nb_msg++;
  // printf("liste des consomatteurs blockés\n");
  print_list(&tab_message_queues[fid]->blocked_consumers);
  update_list(fid, &tab_message_queues[fid]->blocked_consumers, 0);
  //s'il existe des processus consomatteurs bloqués
  if (tab_message_queues[fid]->nb_b_c!=0) {
    // printf("liste des consomatteurs blockés\n");
    print_list(&tab_message_queues[fid]->blocked_consumers);
    //On récupére le processus bloqué le plus prioritaire
    File_priorite *proc;
    proc=queue_out(&tab_message_queues[fid]->blocked_consumers, File_priorite, chaine);
    //On décrément le nombre de processus producteurs bloqués
    tab_message_queues[fid]->nb_b_c--;
    if (table_processus[proc->val]!=NULL) {
      table_processus[proc->val]->etat=activable;
    }
    ordonnancement();
  }
  return 0;
}
int update_list(int fid, link *head, int p) {
  File_priorite *proc_courant = mem_alloc(sizeof(File_priorite));
  int nb;
  if (p==1) {
    nb = tab_message_queues[fid]->nb_b_p;
  }
  else {
    nb = tab_message_queues[fid]->nb_b_c;
  }
  File_priorite *tab_tmp[nb];
  int i_tmp=0;
  while (nb>0) {
    proc_courant=queue_out(head, File_priorite, chaine);
    if (table_processus[proc_courant->val]!=NULL) {
      proc_courant->prio = table_processus[proc_courant->val]->prio;
      tab_tmp[i_tmp] = proc_courant;
      i_tmp++;
    }
    else {
      if (p == 1) {
	tab_message_queues[fid]->nb_b_p--;
      }
      else {
	tab_message_queues[fid]->nb_b_c--;
      }
    }
    nb--;
  }
  for (int j=0; j<i_tmp; j++) {
    queue_add(tab_tmp[j], head, File_priorite, chaine, prio);
  }
  return 0;
}
int print_list(link *head) {
  File_priorite *proc_courant = mem_alloc(sizeof(File_priorite));
  queue_for_each(proc_courant, head, File_priorite, chaine) {
    printf("%d %d \n", proc_courant->val, proc_courant->prio);
  }
  return 0;
}
