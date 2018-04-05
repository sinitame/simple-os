#ifndef _TEST11_H_
#define _TEST11_H_

#include <stdio.h>

//inclure les fichiers de semaphore

struct test11_shared {
        // union sem sem;
        int in_mutex;
};

int proc_mutex(void *arg);

#endif
