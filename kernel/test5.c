/*******************************************************************************
 * Test 5
 *
 * Tests de quelques parametres invalides.
 * Certaines interdictions ne sont peut-etre pas dans la spec. Changez les pour
 * faire passer le test correctement.
 ******************************************************************************/
// #ifdef _TEST5_H_

    #include <stdio.h>
    #include "test5.h"
    #include "processus.h"

    int no_run(void *arg)
    {
            (void)arg;
            assert(0);
            return 1;
    }

    int waiter(void *arg)
    {
            int pid = (int)arg;
            assert(kill(pid) == 0);
            assert(waitpid(pid, 0) < 0);
            return 1;
    }


    int test5(void *arg)
    {
            int pid1, pid2;
            int r;

            (void)arg;

            // Le processus 0 et la priorite 0 sont des parametres invalides
            assert(kill(0) < 0);
            assert(chprio(getpid(), 0) < 0);
            assert(getprio(getpid()) == 128);
            // pid1 = start("no_run", 4000, 64, 0); //version 2A
            pid1 = start(no_run, STACK_LENGTH, 64, "no_run", 0); //version aprenti
            assert(pid1 > 0);
            assert(kill(pid1) == 0);
            assert(kill(pid1) < 0); //pas de kill de zombie
            assert(chprio(pid1, 128) < 0); //changer la priorite d'un zombie
            assert(chprio(pid1, 64) < 0); //changer la priorite d'un zombie
            assert(waitpid(pid1, 0) == pid1);
            assert(waitpid(pid1, 0) < 0);
            // pid1 = start("no_run", 4000, 64, 0); //version 2A
            pid1 = start(no_run, STACK_LENGTH, 64, "no_run", 0); //version aprenti
            assert(pid1 > 0);
            // pid2 = start("waiter", 4000, 65, (void *)pid1); //version 2A
            pid2 = start(waiter, STACK_LENGTH, 92, "waiter", (void *)pid1); //version aprenti
            assert(pid2 > 0);
            assert(waitpid(pid2, &r) == pid2);
            assert(r == 1);
            assert(waitpid(pid1, &r) == pid1);
            assert(r == 0);
            printf("ok.\n");
            return 0;
    }

// #endif
