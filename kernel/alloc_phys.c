#include "alloc_phys.h"
#include <inttypes.h>
#include "mem.h"
#include <stdio.h>
#include "string.h"
// a free list that links all unallocated regions of memory



// free a region of memory
free_list* add(free_list* list, int* address){
  free_list* new = (free_list*) mem_alloc(sizeof(free_list));
  assert(new != NULL);
  new->address = address;
  new->suiv = list;
  list = new;
  return list;
}

// allocate a region of memory, thus deleting region from list, returning the address of the free page
int* delete(free_list* list){
  free_list* first = list;
  list = list->suiv;
  return first->address;
}


// check if a region in unallocated
int is_in_list(free_list* list, int* address){
  free_list* iterator = list;
  if(iterator->address == address){
    return 1;
  }
  while(iterator->suiv != NULL){
    if(iterator->address == address){
      return 1;
    }
    iterator = iterator->suiv;
  }
  return 0;
}
