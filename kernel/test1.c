/*******************************************************************************
 * Test 1
 *
 * Demarrage de processus avec passage de parametre
 * Terminaison normale avec valeur de retour
 * Attente de terminaison (cas fils avant pere et cas pere avant fils)
 ******************************************************************************/

/*
TODO
0) il faudra penser a changer le prototype de start quand on aura la memoire virtuelle
0-bis) le prototype Apprenti est incomplet : on doit pouvoir passer des parametres + le prototype du processus est int (void*)
moral test1.c doit marcher en changeant le prototype de start

1) la terminaison automatique des processus ne marche pas : appeler proc1, proc2 resout le probleme
il faut ajouter le code de terminaison dans la pile lors de la creation des processus

2) il y a aussi un pb sur idle car on ne peut pas executer le code etant donne que l'appel a idle est bloquant
*/


#include "stdio.h"
#include "processus.h"
#define DUMMY_VAL 78

int dummy2(void *arg)
{
        printf("5 ");
        assert((int) arg == DUMMY_VAL + 1);
        return 4;
}

int dummy1(void *arg) {
        printf("1 ");
        assert((int) arg == DUMMY_VAL);
        return 3;
}

int test1(void *arg)
{
    int pid1;
    int r;
    int rval;

    (void)arg;

    pid1 = start(dummy1, 4000, 192, "dummy1", (void *) DUMMY_VAL);
    assert(pid1 > 0);
    printf("2 ");
    r = waitpid(pid1, &rval);
    assert(r == pid1);
    assert(rval == 3);
    printf("3 ");

    pid1 = start(dummy2, 4000, 100, "dummy2", (void *) (DUMMY_VAL + 1));
    assert(pid1 > 0);
    printf("4 ");
    r = waitpid(pid1, &rval);
    assert(r == pid1);
    assert(rval == 4);
    printf("6.\n");
    idle();
    return 0;
}
