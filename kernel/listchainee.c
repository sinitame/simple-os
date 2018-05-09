#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listchainee.h"
#include "processus.h"
#include "mem.h"



Liste ajoutTete(Liste l, int pid, char* nom){
       /* here you can remove the pid from your jobs list */

	Liste liste = mem_alloc(sizeof(*liste));
	liste->pid = pid;
	liste->indice = 0;
	liste->nom_process = nom;
	liste->suiv = l;
	return liste;
};


Liste ajoutQueue(Liste l, int pid, char* nom,int etat){
	int indice = 0;
	Liste tmp = l;
	Liste maillon = mem_alloc(sizeof(*maillon));

	if (!strcmp(nom,"jobs")){
		return l;}


	maillon->pid = pid;
	maillon->fini = etat;
	maillon->time_start = 0;
	maillon->nom_process = mem_alloc(10*sizeof(char));
	maillon->nom_process = strcpy(maillon->nom_process,nom);
	maillon->suiv = NULL;
	if (tmp == NULL) {
		maillon->indice = 0;
		return maillon;
	}

	while (tmp->suiv != NULL) {
		tmp = tmp->suiv;
		indice++;
	}

	indice++;
	maillon->indice = indice;
	tmp->suiv = maillon;
	return l;
};

void afficherListe(Liste l){
	Liste tmp = l;
	int duree = 0;
	while (tmp != NULL){

	if (tmp->fini == 0){
		printf("[%d] en cours d'exécution %s (PID:%d) \n",tmp->indice,tmp->nom_process,tmp->pid);
	}
  else{
		if ((tmp->time_finish != 0) && (tmp->time_start != 0)){
			duree =((tmp->time_finish)-(tmp->time_start));
		}

		printf("[%d] Fini %s après %d ms (PID:%d) \n", tmp->indice,tmp->nom_process,duree,tmp->pid);
	}
	tmp = tmp->suiv;

	}
};

int estVide(Liste l){
	return (l==NULL);
};

Liste supprimerTete(Liste l){
	Liste liste = l;
	l=l->suiv;
	xfree(liste);

	return l;

};


Liste supprimerElement(Liste l, int pid){
	Liste tmp = l;
	Liste previous;

	if (tmp==NULL ){
		return tmp;
	}

	if (tmp->pid == pid){
		l = l->suiv;
		xfree(tmp);
		return l;
	}
	previous = tmp;
	tmp = previous->suiv;
	while (tmp != NULL){
		if (tmp->pid == pid){
			previous->suiv = tmp->suiv;
			xfree(tmp);
			return l;
		}
		previous = tmp;
		tmp = tmp->suiv;
	}
	return l;
}

Liste supprimerElementsFinis(Liste l){
	if (l==NULL){
		return NULL;
	}
	if (l->fini){
		Liste tmp = l->suiv;
		xfree(l);
		return supprimerElementsFinis(tmp);
	}else{
		l->suiv = supprimerElementsFinis(l->suiv);
}		return l;}


Liste changeStatus(Liste l){
	Liste tmp = l;
	//int status;
	while (tmp != NULL) {
		if (table_processus[tmp->pid] == NULL){
			tmp->fini = 1;

		}
		tmp = tmp->suiv;
	}

	return l;

}
