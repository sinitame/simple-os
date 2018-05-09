#ifndef _TEST10_H_
#define _TEST10_H_

    #if defined WITH_SEM

    int test10(void *arg);

    #elif defined WITH_MSG

    int test10(void *arg);

    static void write(int fid, const char *buf, unsigned long len);

    static void read(int fid, char *buf, unsigned long len);

    #else

    // # error WITH_SEM ou WITH_MSG doit être définie.
    #endif

#endif
