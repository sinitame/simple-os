#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include <processus.h>
#include <string.h>
#include "../shared/malloc.c"

extern void ctx_sw(int*,int*);
extern int read_eax();

void init_idle(void){

	/* Initialisation de la table et de la file des processus
	* Le premier (indice 0 est idle), le dernier est le processus tueur (indice NBPROC)
	* Le tableau des donc de taille NBPROC+1
	*/
	for (int i = 0; i<=NBPROC; i++){
		table_processus[i] = NULL;
	}

	//Initialisation de la file de processus
	INIT_LIST_HEAD(&file_processus);


	//Initialisation de idle
	table_processus[0] = malloc(sizeof(Processus));
	Processus *P = table_processus[0];
	P->pid = 0;
	P->reveil=0;
	P->prio = 1;
	strcpy(P->nom,"idle");
	P->etat = actif;
	processus_actif = P;
	queue_add(P, &file_processus,Processus,lien,prio);

}

void init(int pid, const char* nom, unsigned long ssize,int prio, int (*processus)(void*), void *arg){

	//Création du nouveau processus
	table_processus[pid] = malloc(sizeof(Processus));
	Processus *P = table_processus[pid];
	P->pile=malloc(ssize*sizeof(int));
	P->pid = pid;
	P->pere = processus_actif;
	P->prio = prio;
	P->child = NULL;
	strcpy(P->nom,nom);
	P->etat = activable;

	P->pile[ssize-3] = (uint32_t)processus;
	P->pile[ssize-2] = (uint32_t)exit;
	P->pile[ssize-1] = (uint32_t)arg;
	P->registres[esp] = (uint32_t)((P->pile) +ssize-3);

	P->reveil = 0;

	//Ajout du processus à la file des processus
	queue_add(P,&file_processus,Processus,lien,prio);

	//Ajout du processus crée à la liste des fils du process actif
	//la tete de liste est toujours le fils le plus recemment cree
	P->suiv=processus_actif->child;
	if (processus_actif->child != NULL) {
		processus_actif->child->prec=P;
	}
	processus_actif->child=P;
	P->prec=NULL;
	// si la prio du processus cree est superieure a celle du processus actif
	if (prio > processus_actif->prio) {
		ordonnancement();
	}
}

int start(int(*code)(void*), unsigned long ssize, int prio, const char * nom, void *arg){
	if (ssize < (unsigned long)(STACK_LENGTH)-512) {
		for (int futur_pid=0; futur_pid<NBPROC; futur_pid++){
			if (table_processus[futur_pid] == NULL){
				init(futur_pid,nom,ssize+512,prio,code, arg);
				return futur_pid;
			}
		}
	}

	return -1;
}



void idle(void)
{

	for (;;){
		sti();
		hlt();
		cli();
	}
}

void context_switch(Processus *prochain) {
	// un processus ne se repasse pas la main
	Processus *ancien=processus_actif;
	if (prochain != ancien) {
		prochain->etat=actif;
		ancien->etat=(ancien->etat==actif)?activable:ancien->etat;
		// si l'ancien est mort, on le libere
		if (ancien->etat==mort) {
			table_processus[ancien->pid]=NULL;
			free(ancien->pile);
			free(ancien);
		}
		processus_actif = prochain;
		ctx_sw(ancien->registres, prochain->registres);
	}
}

void ordonnancement(void){
	Processus* prochain;
	Processus* top;

	if (!queue_empty(&file_processus)) {
		// parcours de la file dans le sens des prio DECROISSANTE donc queue_for_each_prev
		queue_for_each_prev(top, &file_processus, Processus, lien) {
			// cas de base : le processus_actif est le plus prioritaire et est actif (changement d'etat possible)
			if (top==processus_actif && processus_actif->etat==actif) {
				prochain=top;
				break;
			// reveil des endormis
			} else if (top->etat == endormi && sec>top->reveil){
					top->etat = activable;
				// si zombie avec un pere en attente
			} else if(top->etat == zombie && top->pere-> etat == wait_child){
				top->pere->etat = activable;
			}
			else if (processus_actif->etat==zombie && processus_actif->pere->etat==wait_child) {
				processus_actif->pere->etat=activable;
			}
			// arret de la boucle si on a trouve un processus a activer
			if (top->etat==activable) {
				prochain=top;
				break;
			}
		}
		queue_del(prochain, lien);
		queue_add(prochain, &file_processus, Processus, lien, prio);
	} else {
		// idle
		prochain = table_processus[0];
	}

	context_switch(prochain);

}

void manage_children(Processus *p);

/*
Partie commune a exit et kill
- si pere alors zombie et retvalp=retval
- sinon etat intermediaire mort
*/
void cleaner(Processus *p, int retval) {
	if (p->pere != NULL){
		p->etat = zombie;
		p->retval = retval;
	} else {
		p->etat=mort;
	}
	// "liberation" du pid : le processus "n'existe" plus pour aucune fonction
	// le free sera lors du context_switch pour un processus mort ou dans waitpid pour un zombie
	table_processus[p->pid]=NULL;
	queue_del(p, lien);
	manage_children(p);
	ordonnancement();
}

void exit(int retval){
	retval = read_eax();
	cleaner(processus_actif, retval);
	while(1) {}
}

int kill(int pid){
	if ((pid <= 0) || (pid > NBPROC) || (table_processus[pid] == NULL)){
		return -1;
	// pid valide
	} else {
		cleaner(table_processus[pid], 0);
		return 0;
	}
}

/*
Lors de la mort de'un processus,
celui-ci doit etre enleve de la liste des fils de son pere
*/
void manage_childlist(Processus *P) {
	if (P->pere != NULL) {
		Processus *child = P->pere->child;
		while (child != NULL && child->pid != P->pid) {
			child=child->suiv;
		}
		// suppression de la tete
		if (child->prec==NULL) {
			P->pere->child=child->suiv;
		} else {
			child->prec->suiv=child->suiv;
		}
		if (child->suiv != NULL) {
			child->suiv->prec=child->prec;
		}
	}
}

/*
Lors de la mort du pere, les fils doivent :
- etre detruits s'ils sont zombies
- etre marques comme n'ayant plus de pere
*/
void manage_children(Processus *P){
	Processus* child = P->child;
	// pour ne pas scier la branche sur laquelle on est
	Processus *tmp;
	while (child != NULL){
		tmp = child->suiv;
		// si on tombe sur un zombie, il doit etre detruit car aucun pere ne l'attendra
		if (child->etat == zombie){
			free(child->pile);
			free(child);
		// sinon il n'y a plus de pointeur sur le pere
		} else {
			child->pere = NULL;
		}
		child = tmp;
	}
}

int destroy_child(Processus *child, int *retvalp) {
	int pid=child->pid;
	if (retvalp!=NULL) {*retvalp = child->retval;}
	// on l'enleve de la liste des fils de son pere
	manage_childlist(child);
	// enfin on libere le pid
	free(child->pile);
	free(child);
	return pid;
}

int waitpid(int pid, int *retvalp){
	Processus * child = processus_actif->child;

	if (child == NULL){
		return -1;
	} else if (pid <0){
		while (1) {
			while(child !=NULL){
				if (child->etat == zombie){
					return destroy_child(child, retvalp);
				}
				child = child->suiv;
			}
			processus_actif->etat = wait_child;
			ordonnancement();
			// retour au debut pour attente active
			child=processus_actif->child;
		}
	} else {
		while(child!=NULL){
			if(child->pid == pid){
				while (1) {
					if(child->etat == zombie){
						return destroy_child(child, retvalp);
					}
					processus_actif->etat = wait_child;
					ordonnancement();
				}
			}
			child=child->suiv;
		}
	}
	return -1;
}

int getprio(int pid){
	if (pid<=0 || pid>NBPROC || table_processus[pid] == NULL){
		return -1;
	} else {
		return table_processus[pid]->prio;
	}
}


int chprio(int pid, int newprio){
	if (pid<=0 || pid>NBPROC || table_processus[pid] == NULL || newprio <=0 || newprio > MAXPRIO){
		return -1;
	} else {
		queue_del(table_processus[pid], lien);
		int ancienne_prio = table_processus[pid]->prio;
		table_processus[pid]->prio = newprio;
		queue_add(table_processus[pid], &file_processus, Processus, lien, prio);
		ordonnancement();
		return ancienne_prio;
	}
}

int getpid(void){
	return processus_actif->pid;
}

char* mon_nom(void){
	return processus_actif->nom;
}

void wait_clock(uint32_t nbr_secs){
	processus_actif->reveil += nbr_secs;
	processus_actif->etat = endormi;
	ordonnancement();
}
