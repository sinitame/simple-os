#ifndef LIST_CHAINEE_H
#define LIST_CHAINEE_H

#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
//#include <sys/time.h>

struct maillon{
	int pid;
	int indice;
	int time_start;
	int time_finish;
	int fini;
	char *nom_process;
	struct maillon *suiv;
};

typedef struct maillon *Liste;

Liste ajoutTete(Liste l, int pid, char* nom);
Liste ajoutQueue(Liste l, int pid, char* nom,int etat);
void afficherListe(Liste l);
int estVide(Liste l);
Liste supprimerTete(Liste l);
Liste supprimerElementsFinis(Liste l);
Liste changeStatus(Liste l);

#endif
