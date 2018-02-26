#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include "../shared/queue.h"


#define NBPROC 10
#define MAXPRIO 256

enum registre_type{ebx=0,esp=1,ebp=2,esi=3,edi=4}; //index du tableau
enum etats {actif,activable,wait_sem,wait_io,wait_child,endormi,zombie,mort};

typedef struct Processus Processus;
typedef struct Element *Liste;

struct Element {
	int pid;
	Liste suiv ;
};

struct Processus {
	int pid;
	int prio;
	Processus *pere;
	int retval;
	Liste childs;
	char nom[15];
	enum etats etat;
	int registres[5];
	int pile[512];
	int reveil;
	link lien;
};


Processus *table_processus[NBPROC+1];
link file_processus;
Processus *processus_actif;

//Fonction d'initialisation de idle
void init_idle(void);

//Fonction d'initialisation d'un processus
void init(int pid, char* nom, int etat,int prio, void (*processus)(void));

/*
/Fonction de creation d'un processus.
/TODO : jouter l'argument paramètre 
*/
uint32_t start(void(*code)(void), char * nom, int taille_pile, int prio);

//Fonction crées pour les tests
void idle(void);
void tueur(void);
void proc1(void);
void proc2(void);
void proc3(void);
void proc4(void);

//Fonctions gérant l'ordonnancement des processus
void ordonnancement_simple(void);
void ordonnancement(void);

//Primitives de gestion des processus comme spécifié dans la spec
void exit1(int retval);
int kill(int pid);
int waitpid(int pid, int *retvalp);
int getprio(int pid);
int chprio(int pid, int newprio);
int getpid(void);

//Fonctions annexes utiles
int mon_pid(void);
char * mon_nom(void);
void kill_childs(Processus *P);
void dors(uint32_t nbr_secs);
