#include alloc_phys.h
// a free list that links all unallocated regions of memory



// free a region of memory
free_list* add(free_list* list, int* address){
  free_list* new = (free_list*) malloc(sizeof(free_list));
  assert(new != NULL);
  new->address = address;
  new->suiv = list;
  list = new;
  return list;
}

// allocate a region of memory
free_list* delete(free_list* list, int* address){
  free_list* iterator = list;
  if(list == NULL){
    return NULL;
  }

  if(iterator->address == address){
    list = iterator->suiv;
    free(iterator);
    return list;
  }
  iterator_next = iterator->suiv;
  while(iterator_next != NULL){
    if(iterator_next->address == address){
      iterator-> suiv = iterator_next->suiv;
      free(iterator_next);
      return list;
    }
    iterator = iterator_next;
    iterator_next = iterator_next->suiv;
  }
  return list;
}


// check if a region in unallocated
boolean is_in_list(free_list* list, int* address){
  iterator = list;
  if(iterator->address == address){
    return true;
  }
  while(iterator->suiv != null){
    if(iterator->address == address){
      return true;
    }
    iterator = iterator->suiv;
  }
  return false;
}
