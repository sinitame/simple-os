#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "queue.h"

#define NBQUEUE 1

struct file_priorite {
  int val;
  int prio;
  link chaine;
};
typedef struct file_priorite File_priorite;

struct message_queue {
  link blocked_producers;
  int nb_b_p;
  link blocked_consumers;
  int nb_b_c;
  int prio_c;
  int queue_capacity;
  int nb_msg;
  link messages;
};
typedef struct message_queue Message_queue;

extern int nb_queues;
extern Message_queue *tab_message_queues[NBQUEUE];

int pcount(int fid, int *count);
int pcreate(int count);
int pdelete(int fid);
int preceive(int fid,int *message);
int preset(int fid);
int psend(int fid, int message);
#endif
