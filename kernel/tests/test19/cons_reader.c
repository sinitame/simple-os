#ifdef TEST19_H

#include "test19.h"

int cons_reader(void *arg)
{
        int fid = (int)arg;
        int c = cons_read();
        assert(psend(fid, 1) == 0);
        printf(" %d (%c)", 134 - getprio(getpid()), c);
        return 0;
}

#endif 
