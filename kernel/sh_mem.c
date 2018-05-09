#include "hash.h"
#include "stddef.h"
#include "mem.h"
#include "sh_mem.h"

hash_t *map;

void *shm_create(const char *key) {

// HASH_INIT is defined if a hash map is set
#ifndef HASH_INIT
    map=mem_alloc(sizeof(hash_t));
    hash_init_string(map);
    #define HASH_INIT
#endif

    void* page=mem_alloc(4*1024);
    if (!hash_set(map, (void*)key, page)) {
        return page;
    }
    mem_free(page, 4*1024);
    return NULL;
}

void *shm_acquire(const char *key) {
    return hash_get(map, (void*)key, NULL);
}

void shm_release(const char *key) {
        // TODO faux car la page peut encore etre pointee par des processus
        // mais ok pour l'instant
        if (!hash_isset(map, (void*)key)) {
            hash_del(map, (void*)key);

        }
}
