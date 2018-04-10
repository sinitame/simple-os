#include "processus.h"
#include "stdio.h"
#include "stddef.h"

int child1(void * args) {
	(void)args;
	printf(" 2");
	return 0;
}

int child2(void *args) {
	(void) args;
	return 0;
}

int father(void * args) {
	// child1 s'execute car il est de prio superieure a father puis passe zombie
	// child2 non
	start(child1, STACK_LENGTH, 200, "child1", (void *) args);
	printf(" 3");
	start(child2, STACK_LENGTH, 32, "child2", (void *) args);
	printf(" 4");
	// change sa prio pour forcer le scheduler a choisir killer
	chprio(getpid(), 92);
	return 0;
}

int killer(void *args)
{
		int pid1;

		printf("1");
		pid1=start(father, STACK_LENGTH, 192, "father", (void *) args);
		printf(" 5");
		assert(table_processus[3]->etat==zombie && table_processus[4]->etat==activable);
		// killer tue father qui a deux fils : child1 (zombie car il s'est execute) et child2 (encore activable)
		kill(pid1);
		Processus* mystere=queue_entry(((Processus*)queue_bottom(&file_processus, Processus, lien))->lien.next, Processus, lien);
		// le processus qui suit idle est child2
		assert(mystere==table_processus[4]);
		// le haut de la file est occupe par killer car father et child1 ont ete enleves
		assert(queue_top(&file_processus, Processus, lien)==table_processus[1]);
		printf(" 6.\n");
		return 0;
}
