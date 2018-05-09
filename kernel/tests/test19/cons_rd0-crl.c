#ifdef TEST19_H

#include "test19.h"

int cpns_rd0(void *arg)
{
        unsigned long i;
        char buf[101];

        (void)arg;

        i = cons_read(buf, 100);
        buf[i] = 0;
        printf(". : %s\n", buf);
        return 0;
}

#endif
