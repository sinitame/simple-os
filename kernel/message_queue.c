#include <stdio.h>
#include <string.h>
#include "message_queue.h"
#include "mem.h"
#include "processus.h"

int nb_queues=0;
Message_queue *tab_message_queues[NBQUEUE]={NULL};

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
  if (nb_queues>=NBQUEUE || count<=0) {
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
  //Parcourir la file de messages
  File_priorite *curr_element;
  queue_for_each(curr_element, &tab_message_queues[fid]->messages, File_priorite, chaine) {
    //supprimer tous les messages dans la file
    queue_del(curr_element, chaine);
    mem_free(curr_element, sizeof(File_priorite));
  }
  //Parcourir la file des processus consommateurs
  queue_for_each(curr_element, &tab_message_queues[fid]->blocked_consumers, File_priorite, chaine) {
    //Supprimer les élements de la file
    queue_del(curr_element, chaine);
    //Libérer les processus consommateurs
    table_processus[curr_element->val]->etat=activable;
    mem_free(curr_element,sizeof(File_priorite));
  }
  //Parcourir la file des processus producteurs
  queue_for_each(curr_element, &tab_message_queues[fid]->blocked_producers, File_priorite, chaine) {
    //Supprimer les élements de la file
    queue_del(curr_element, chaine);
    //Libérer les processus producteurs
    table_processus[curr_element->val]->etat=activable;
    mem_free(curr_element,sizeof(File_priorite));
  }
  mem_free(tab_message_queues[fid], sizeof(Message_queue));
  nb_queues--;
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
  }
  //s'il existe au moins un message dans la file
  else {
    //On extrait le message le plus ancien dans la file et on le place dans "message"
    File_priorite *msg;
    msg=queue_bottom(&tab_message_queues[fid]->messages, File_priorite, chaine);
    queue_del(msg, chaine);
    if (message!=NULL) {
      *message=msg->val;
    }
    mem_free(msg, sizeof(File_priorite));
    //On décrément le nombre de messages dans la file
    tab_message_queues[fid]->nb_msg--;
    //s'il existe des processus producteurs bloqués
    if (!queue_empty(&tab_message_queues[fid]->blocked_producers)) {
      //On récupére le processus bloqué le plus ancien
      File_priorite *proc;
      proc=queue_out(&tab_message_queues[fid]->blocked_producers, File_priorite, chaine);
      //On décrément le nombre de processus producteurs bloqués
      tab_message_queues[fid]->nb_b_p--;
      table_processus[proc->val]->etat=activable;
      ordonnancement();
    }
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
  //Parcourir la file de messages
  File_priorite *curr_element;
  queue_for_each(curr_element, &tab_message_queues[fid]->messages, File_priorite, chaine) {
    //supprimer tous les messages dans la file
    queue_del(curr_element, chaine);
    mem_free(curr_element, sizeof(File_priorite));
  }
  //Parcourir la file des processus consommateurs
  queue_for_each(curr_element, &tab_message_queues[fid]->blocked_consumers, File_priorite, chaine) {
    //Supprimer les élements de la file
    queue_del(curr_element, chaine);
    table_processus[curr_element->val]->etat=activable;
    mem_free(curr_element, sizeof(File_priorite));
  }
  //Parcourir la file des processus producteurs
  queue_for_each(curr_element, &tab_message_queues[fid]->blocked_producers, File_priorite, chaine) {
    //Supprimer les élements de la file
    queue_del(curr_element, chaine);
    table_processus[curr_element->val]->etat=activable;
    mem_free(curr_element, sizeof(File_priorite));
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
  }
  // Si la file de messages n'est pas pleine
  else {
    File_priorite *nv_msg;
    nv_msg=mem_alloc(sizeof(File_priorite));
    nv_msg->val=message;
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
    //s'il existe des processus consomatteurs bloqués
    if (tab_message_queues[fid]->nb_b_c!=0) {
      //On récupére le processus bloqué le plus ancien parmi les plus prioritaires
      File_priorite *proc;
      proc=queue_out(&tab_message_queues[fid]->blocked_consumers, File_priorite, chaine);
      //On décrément le nombre de processus consomatteurs bloqués
      tab_message_queues[fid]->nb_b_c--;
      table_processus[proc->val]->etat=activable;
      ordonnancement();
    }
  }
  return 0;
}
