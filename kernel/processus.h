#ifndef __PROCESSUS_H__
#define __PROCESSUS_H__

#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include "../shared/queue.h"
#include "../user/lib/clock.h"

#define NBPROC 10
#define MAXPRIO 256
#define STACK_LENGTH 512

enum registre_type{ebx=0,esp=1,ebp=2,esi=3,edi=4}; //index du tableau
enum etats {actif,activable,wait_sem,wait_io,wait_child,endormi,zombie,mort};

typedef struct Processus Processus;

struct Processus {
	int pid;
	int prio;
	int retval;
	enum etats etat;
	uint32_t reveil;
	char nom[15];
	int registres[5];
	int pile[STACK_LENGTH];
	Processus *pere;
	// pour la file de prio
	link lien;

	// pour la liste des fils
	// liste doublement chainee non circulaire
	// tete de liste = p->child
	Processus *child;
	Processus *prec;
	Processus *suiv;
};


Processus *table_processus[NBPROC+1];
link file_processus;
Processus *processus_actif;

//Fonction d'initialisation de idle
void init_idle(void);

//Fonction d'initialisation d'un processus
void init(int pid, const char* nom, unsigned long ssize,int prio, int (*processus)(void*), void *arg);
/*
/Fonction de creation d'un processus.
/TODO : jouter l'argument paramètre
*/
uint32_t start(int(*code)(void*), unsigned long taille_pile, int prio, const char * nom, void *arg);
//Fonction crées pour les tests
void idle(void);

void ordonnancement(void);

//Primitives de gestion des processus comme spécifié dans la spec
void exit(int retval);
int kill(int pid);
int waitpid(int pid, int *retvalp);
int getprio(int pid);
int chprio(int pid, int newprio);
int getpid(void);

//Fonctions annexes utiles
char * mon_nom(void);
void kill_childs(Processus *P);
void wait_clock(uint32_t nbr_secs);

#endif
