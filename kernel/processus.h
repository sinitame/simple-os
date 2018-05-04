#ifndef __PROCESSUS_H__
#define __PROCESSUS_H__

#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include "../shared/queue.h"
#include "../user/lib/clock.h"
#include "hash.h"
#include "userspace_apps.h"

#define NBPROC 10
#define MAXPRIO 256

#define STACK_KERNEL 1024	// taille mini de 1ko

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
	int *pile;
	Processus *pere;
	// pour la file de prio
	link lien;

	// pour la liste des fils
	// liste doublement chainee non circulaire
	// tete de liste = p->child
	Processus *child;
	Processus *prec;
	Processus *suiv;
	unsigned* pgdir ;
	unsigned* pgtab ;
};


Processus *table_processus[NBPROC+1];
link file_processus;
Processus *processus_actif;

// idle
void init_idle(void);
void idle(void);

int start(int(*code)(void*), unsigned long taille_pile, int prio, const char * nom, void *arg);
void ordonnancement(void);

void exit(int retval);
int kill(int pid);
int waitpid(int pid, int *retvalp);
int getprio(int pid);
int chprio(int pid, int newprio);
int getpid(void);
void wait_clock(uint32_t nbr_secs);
hash_t* create_hash();
void proc_mapping(unsigned *pgdir, unsigned virtual_adress, unsigned physical_adress, unsigned permission);

#endif
