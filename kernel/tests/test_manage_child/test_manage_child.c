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
	start(child1, 4000, 200, "child1", (void *) args);
	printf(" 3");
	start(child2, 4000, 32, "child2", (void *) args);
	printf(" 4");
	// change sa prio pour forcer le scheduler a choisir killer
	chprio(getpid(), 92);
	return 0;
}

int killer(void *args)
{
		int pid1;

		printf("1");
		pid1=start(father, 4000, 192, "father", (void *) args);
		printf(" 5");
		// killer tue father qui a deux fils : child1 (zombie car il s'est execute) et child2 (encore activable)
		kill(pid1);
		printf(" 6.\n");
		return 0;
}
