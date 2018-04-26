#ifndef __START_H__
#define __START_H__

#define FIRST_STACK_SIZE 16384

#ifndef ASSEMBLER

extern unsigned char first_stack[FIRST_STACK_SIZE];
extern void traitant_IT_32();
extern void traitant_IT_33();

/* This is the first user process to start. It is mapped to the entry point
of the user code. */
int user_start(void *);

/* The kernel entry point */
void kernel_start(void);

#endif

#endif
