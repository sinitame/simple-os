/*
 * Copyright (C) 2005 -- Simon Nieuviarts
 * Copyright (C) 2012 -- Damien Dejean <dam.dejean@gmail.com>
 *
 * Kernel memory allocator.
 */
#include "mem.h"
#include "types.h"

/* Heap boundaries */
extern char mem_heap[];
extern char mem_heap_end[];
static char *curptr = mem_heap;

void xfree(void *zone){
	mem_free_nolength(zone);
};

void *xrealloc(void *ptr, size_t size){
	void *p = mem_alloc(size);
	if(ptr == NULL){
		return p;
	}

	memcpy(p, ptr, size);

	xfree(ptr);
	return p;
}


/* Trivial sbrk implementation */
void *sbrk(ptrdiff_t diff)
{
	char *s = curptr;
	char *c = s + diff;
	if ((c < curptr) || (c > mem_heap_end)) return ((void*)(-1));
	curptr = c;
	return s;
}
