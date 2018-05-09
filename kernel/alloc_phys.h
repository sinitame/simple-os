#ifndef ALLOC_H
#define ALLOC_H


typedef struct free_list{
  unsigned* address;
  struct free_list *suiv;
} free_list;


free_list* create_list();
free_list* add(free_list* list, unsigned* address);
unsigned* delete(free_list* list);
unsigned is_in_list(free_list* list, unsigned* address);

#endif
