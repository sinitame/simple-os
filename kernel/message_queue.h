#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "queue.h"

#define NBQUEUE 1

struct file_priorite {
  int32_t val;
  int32_t prio;
  link chaine;
};
typedef struct file_priorite File_priorite;

struct message_queue {
  link blocked_producers;
  int32_t nb_b_p;
  link blocked_consumers;
  int32_t nb_b_c;
  int32_t queue_capacity;
  int32_t nb_msg;
  link messages;
};
typedef struct message_queue Message_queue;
  /*pthread_mutex_t mutex;
  pthread_cond_t fp;
  pthread_cond_t fc;
  int32_t nb_b_p;
  int32_t nb_b_c;
  int32_t queue_capacity;
  int32_t nb_msg;
  link messages;*/

extern int32_t nb_queues;
extern Message_queue *tab_message_queues[NBQUEUE];

int pcount(int fid, int *count);
int pcreate(int count);
int pdelete(int fid);
int preceive(int fid,int *message);
int preset(int fid);
int psend(int fid, int message);
#endif
