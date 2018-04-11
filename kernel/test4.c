    #ifdef _TEST4_H_

    #include "stdio.h"
    #include "tests.h"
    #include "processus.h"
    #include "../user/tests/lib/it.c"


/*
    void test_it(void)
    {
    #ifdef microblaze
            int status, mstatus;
            __asm__ volatile("mfs %0,rmsr; ori %1,%0,2; mts rmsr,%1; nop; nop; mts rmsr,%0":"=r" (status), "=r" (mstatus));
    #else
            __asm__ volatile("pushfl; testl $0x200,(%%esp); jnz 0f; sti; nop; cli; 0: addl $4,%%esp\n":::"memory");
    #endif
  }*/


    int busy1(void *arg)
    {
            (void)arg;
            int k=0;
            while (1) {

                    int i, j;

                    printf(" A");
                    for (i=0; i<loop_count1; i++) {
                            test_it();
                            for (j=0; j<loop_count0; j++);
                    }
                    k++;
            }
            return 0;
    }


    /* assume the process to suspend has a priority == 64 */
    int busy2(void *arg)
    {
            int i;

            for (i = 0; i < 3; i++) {
                    int k, j;

                    printf(" B");
                    for (k=0; k<loop_count1; k++) {
                            test_it();
                            for (j=0; j<loop_count0; j++);
                    }
            }
            i = chprio((int) arg, 16);
            assert(i == 64);
            return 0;
    }

    int test4(void *args)
    {
            int pid1, pid2;
            int r;
            int arg = 0;

            (void)args;

            assert(getprio(getpid()) == 128);
            pid1 = start(busy1, STACK_LENGTH, 64, "busy1", (void *) arg);
            assert(pid1 > 0);
            pid2 = start(busy2, STACK_LENGTH, 64, "busy2", (void *) pid1);
            assert(pid2 > 0);
            printf("1 -");
            r = chprio(getpid(), 32);
            assert(r == 128);
            printf(" - 2");
            r = kill(pid1);
            assert(r == 0);
            assert(waitpid(pid1, 0) == pid1);
            r = kill(pid2);
            assert(r < 0); /* kill d'un processus zombie */
            assert(waitpid(pid2, 0) == pid2);
            printf(" 3");
            r = chprio(getpid(), 128);
            assert(r == 32);
            printf(" 4.\n");

            return 0;
    }

    #endif
