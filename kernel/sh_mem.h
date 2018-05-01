#ifndef SH_MEM_H
#define SH_MEM_H

extern void *shm_create(const char *key);
extern void *shm_acquire(const char *key);
extern void shm_release(const char *key);

#endif
