#include "sh_mem.h"
#include "message_queue.h"
#include "psender.h"
#include "stddef.h"
#include "../shared/string.h"

int psender(void *arg)
{
        struct psender *ps = NULL;
        int ps_index = (int)arg;
        unsigned i;
        unsigned n;

        ps = shm_acquire("test13_shm");
        assert(ps != NULL);
        n = strlen(ps[ps_index].data);

        for(i = 0; i < n; i++) {
                assert(psend(ps[ps_index].fid, ps[ps_index].data[i]) == 0);
        }
        shm_release("test13_shm");
        return 0;
}
