/*******************************************************************************
 * Test 7
 *
 * Test de l'horloge (ARR et ACE)
 * Tentative de determination de la frequence du processeur et de la
 * periode de scheduling
 ******************************************************************************/

    #ifdef TELECOM_TST
    int test7(void *arg)
    {
            (void)arg;
            printf("Test desactive pour les TELECOM.\n");
    }

    #else
    #include <stdio.h>
    #include "processus.h"
    #include "hash.h"
    #include "stddef.h"
    #include "mem.h"

    void* shm_acquire(const char *key);
    extern void test_it();

    int sleep_pr1(void *arg)
    {
            (void)arg;
            wait_clock(current_clock() + 2);
            printf(" not killed !!!");
            assert(0);
            return 1;
    }

    int timer2(void *arg)
    {
            volatile unsigned long *timer = NULL;
            timer = shm_acquire("test7_shm");
            assert(timer != NULL);

            (void)arg;
            while (1) {
                    unsigned long t = *timer + 1;
                    *timer = t;
                    while (*timer == t) test_it();
            }
            while (1);
            return 0;
    }

    int timer1(void *arg)
    {
            (void)arg;

            unsigned long quartz;
            unsigned long ticks;
            unsigned long dur;
            int i;

            clock_settings(&quartz, &ticks);
            dur = (quartz + ticks) / ticks;
            printf(" 2");
            for (i = 4; i < 8; i++) {
                    wait_clock(current_clock() + dur);
                    printf(" %d", i);
            }
            return 0;
    }

    hash_t *map;

    void *shm_create(const char *key) {
        map = mem_alloc(sizeof(hash_t));
        hash_init_string(map);
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
            hash_del(map, (void*)key);
    }

    int test7(void *arg)
    {
            int pid1;
            int pid2;
            int r;
            unsigned long c0, c, quartz, ticks, dur;
            volatile unsigned long *timer = NULL;

            (void)arg;
            timer = shm_create("test7_shm");
            assert(timer != NULL);

            assert(getprio(getpid()) == 128);
            printf("1");
            pid1 = start(timer1, 4000, 129, "timer1", 0);
            assert(pid1 > 0);
            printf(" 3");
            assert(waitpid(-1, 0) == pid1);
            printf(" 8\n");

            *timer = 0;
            pid1 = start(timer2, 4000, 127, "timer", 0);
            pid2 = start(timer2, 4000, 127, "timer", 0);
            assert(pid1 > 0);
            assert(pid2 > 0);
            clock_settings(&quartz, &ticks);
            dur = 2 * quartz / ticks;
            test_it();
            c0 = current_clock();
            do {
                    test_it();
                    c = current_clock();
            } while (c == c0);
            wait_clock(c + dur);
            assert(kill(pid1) == 0);
            assert(waitpid(pid1, 0) == pid1);
            assert(kill(pid2) == 0);
            assert(waitpid(pid2, 0) == pid2);
            printf("%lu changements de contexte sur %lu tops d'horloge", *timer, dur);
            pid1 = start(sleep_pr1, 4000, 192, "sleep_pr1", 0);
            assert(pid1 > 0);
            assert(kill(pid1) == 0);
            assert(waitpid(pid1, &r) == pid1);
            assert(r == 0);
            printf(".\n");
            shm_release("test7_shm");
            return 0;
    }
    #endif
