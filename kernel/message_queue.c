#include "message_queue.h"

int32_t nb_queues=0;
Message_queue tab_message_queues[NBQUEUE]={NULL};

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
    if (queue_empty(tab_message_queues[fid].head_messages)==0) {
      *count = -tab_message_queues[fid].nb_b_c;
    }
    else if {
      *count = tab_message_queues[fid].nb_msg + tab_message_queues[fid].nb_b_p;
    }
  }
  return 0;
}

int pcreate(int count) {
  //Il reste plus de files de messages ou le nombre count est négative ou égale à zéro
  if (nb_queues>=NBQUEUE || count<=0) {
    return -1;
  }

  int fid=0;
  boolean available_queue=false;
  //Chercher une file non utilisable
  while (fid<NBQUEUE && !available_queue) {
    if (tab_message_queues[fid]==NULL) {
      available_queue = true;
    }
    else {
      fid++;
    }
  }
  tab_message_queue[fid] = malloc(sizeof(Message_queue));
  tab_message_queues[fid]->nb_msg=0;
  tab_message_queues[fid]->nb_b_c=0;
  tab_message_queues[fid]->nb_b_p=0;
  //initialiser le nombre de messages à count
  tab_message_queues[fid]->queue_capacity=count;
  //initialiser la file des processus producteurs
  tab_message_queues[fid]->blocked_producers = LIST_HEAD_INIT(tab_message_queues[fid]->blocked_producers);
  //initialiser la file des processus consomatteurs
  tab_message_queues[fid]->blocked_consumers = LIST_HEAD_INIT(tab_message_queues[fid]->blocked_consumers);
  //initialiser la file des messages
  tab_message_queues[fid]->messages = LIST_HEAD_INIT(tab_message_queues[fid]->messages);
  //Incrémenter le nombre de files de messages utilisés
  nb_queues++;
  return fid;
}

int pdelete(int fid) {
  if (fid>NBQUEUE || fid<0) {
    return -1;
  }
  //Parcourir la file de messages
  File_priorite *curr_element;
  queue_for_each(curr_element, &tab_message_queues[fid].messages, File_priorite, chaine) {
    //supprimer tous les messages dans la file
    queue_del(curr_element, chaine);
    free(curr_element);
  }
  free(tab_message_queues[fid]);
  nb_queues--;
  return 0; 
}

int preceive(int fid,int *message) {
  pthread_mutex_lock(&(tab_message_queues[fid]->mutex));
  if (tab_message_queues[fid]->nb_msg==0) {
    tab_message_queues[fid]->nb_b_c++;
    pthread_cond_wait(&tab_message_queues[fid]->fc, &tab_message_queues[fid]->mutex);
  }
  tab_message_queues[fid]->nb_b_c--;
  tab_message_queues[fid]->nb_msg--;
  File_priorite msg;
  msg=queue_out(&(tab_message_queues[fid].messages), File_priorite, chaine);
  *message=msg->val;
  pthread_cond_signal(&tab_message_queues[fid]->fc);
  pthread_mutex_unlock(&tab_message_queues[fid]->mutex);
}

int preset(int fid) {
}

int psend(int fid, int message){
  if (tab_message_queues[fid]->nb_msg<tab_message_queues[fid]->queue_capacity) {
    tab_message_queues[fid]->nb_msg++;
    File_priorite *nv_msg;
    nv_msg=malloc(sizeof(File_priorite));
    nv_msg->val=message;
    nv_msg->prio=0;
    INIT_LIST_HEAD(&(nv_msg->chaine));
    queue_add(nv_msg, &(tab_message_queues[fid]->messages), File_priorite, chaine, prio);
  }
}


  
  /*pthread_mutex_lock(&(tab_message_queues[fid]->mutex));
  if (tab_message_queues[fid]->nb_msg==tab_message_queues[fid]->queue_capacity) {
    tab_message_queues[fid]->nb_b_p++;
    pthread_cond_wait(&tab_message_queues[fid]->fp, &tab_message_queues[fid]->mutex);
  }
  tab_message_queues[fid]->nb_b_p--;
  tab_message_queues[fid]->nb_msg++;
  File_priorite *nv_msg;
  nv_msg=malloc(sizeof(File_priorite));
  nv_msg->val=message;
  nv_msg->prio=0;
  INIT_LIST_HEAD(&(nv_msg->chaine));
  queue_add(nv_msg, &(tab_message_queues[fid]->messages), File_priorite, chaine, prio);
  pthread_cond_signal(&tab_message_queues[fid]->fc);
  pthread_mutex_unlock(&tab_message_queues[fid]->mutex);*/
}
