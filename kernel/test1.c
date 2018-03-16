/*******************************************************************************
 * Test 1
 *
 * Demarrage de processus avec passage de parametre
 * Terminaison normale avec valeur de retour
 * Attente de terminaison (cas fils avant pere et cas pere avant fils)
 ******************************************************************************/

#include "stdio.h"
#include "processus.h"
#define DUMMY_VAL 78

void dummy2(void)
{
        printf(" 5");
        // assert((int) arg == DUMMY_VAL + 1);
        // return 4;
}

void dummy1(void) {
        printf("1");
        // assert((int) arg == DUMMY_VAL);
        // return 3;
}


int test1(void *arg)
{
        // int pid1;
        // int r;
        // int rval;

        (void)arg;

        init_idle();
        start(dummy1, "dummy1", 4000, 192);
        start(dummy2, "dummy2", 4000, 100);
        idle();
        // assert(pid1 > 0);
        // printf(" 2");
        // r = waitpid(pid1, &rval);
        // assert(r == pid1);
        // assert(rval == 3);
        // printf(" 3");
        // pid1 = start(dummy2, "dummy2", 4000, 100);
        // assert(pid1 > 0);
        // printf(" 4");
        // r = waitpid(pid1, &rval);
        // assert(r == pid1);
        // assert(rval == 4);
        // printf(" 6.\n");
        return 0;
}
