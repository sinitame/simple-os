#ifndef ALLOC_H
#define ALLOC_H





typedef struct free_list{
  int* address;
  struct free_list *suiv;
} free_list;

#endif
