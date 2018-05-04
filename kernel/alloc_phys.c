#include "alloc_phys.h"
#include <inttypes.h>
#include "mem.h"
#include <stdio.h>
#include "string.h"
// a free list that links all unallocated regions of memory


free_list* create_list(){
  free_list* new = NULL;
  return free_list;
}



// free a region of memory
free_list* add(free_list* list, unsigned* address){
  assert(adress % 4096 ==0); // Assuring that page adress is a multiple of 4096
  free_list* new = (free_list*) mem_alloc(sizeof(free_list));
  assert(new != NULL);
  new->address = address;
  new->suiv = list;
  list = new;
  return list;
}

// allocate a region of memory, thus deleting region from list, returning the address of the free page
unsigned* delete(free_list* list){
  free_list* first = list;
  list = list->suiv;
  return first->address;
}


// check if a region in unallocated
unsigned is_in_list(free_list* list, unsigned* address){
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
