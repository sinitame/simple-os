/*******************************************************************************
 * Test 6 kernel
 *
 * Waitpid multiple.
 * Creation de processus avec differentes tailles de piles.
 *******************************************************************************/
// #ifdef _TEST6_H_

#include <stdio.h>
#include "processus.h"
__asm__(
".text\n"
".globl proc6_1\n"
"proc6_1:\n"
"movl $3,%eax\n"
"ret\n"
".previous\n"
);

__asm__(
".text\n"
".globl proc6_2\n"
"proc6_2:\n"
"movl 4(%esp),%eax\n"
"pushl %eax\n"
"popl %eax\n"
"ret\n"
".previous\n"
);

__asm__(
".text\n"
".globl proc6_3\n"
"proc6_3:\n"
"movl 4(%esp),%eax\n"
"pushl %eax\n"
"popl %eax\n"
"ret\n"
".previous\n"
);

extern int proc6_1(void* args);
extern int proc6_2(void* args);
extern int proc6_3(void* args);


int test6(void *arg)
{
        int pid1, pid2, pid3;
        int ret;

        (void)arg;

        assert(getprio(getpid()) == 128);
        pid1 = start(proc6_1, 0, 64, "proc6_1", 0);
        assert(pid1 > 0);
        pid2 = start(proc6_2, 4, 66,"proc6_2", (void*)4);
        assert(pid2 > 0);
        pid3 = start(proc6_3, 0xffffffff, 65, "proc6_3", (void*)5);
        assert(pid3 < 0);
        pid3 = start(proc6_3, 8, 65, "proc6_3", (void*)5);
        assert(pid3 > 0);
        assert(waitpid(-1, &ret) == pid2);
        assert(ret == 4);
        assert(waitpid(-1, &ret) == pid3);
        assert(ret == 5);
        assert(waitpid(-1, &ret) == pid1);
        assert(ret == 3);
        assert(waitpid(pid1, 0) < 0);
        assert(waitpid(-1, 0) < 0);
        assert(waitpid(getpid(), 0) < 0);
        printf("ok.\n");
        return 0;
}

//#endif
