#include "sh_mem.h"
#include "stddef.h"
#include "test16.h"
#include "message_queue.h"

extern void test_it(void);

int proc16_3(void *arg)
{
        struct tst16 *p = NULL;
        int i;

        (void)arg;
        p = shm_acquire("test16_shm");
        assert(p != NULL);

        for (i = 0; i < p->count; i++) {
                assert(psend(p->fid, i) == 0);
                test_it();
        }
        shm_release("test16_shm");
        return 0;
}
