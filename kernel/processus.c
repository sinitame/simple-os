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
	P->pid = pid;
	P->pere = processus_actif;
	P->prio = prio;
	P->child = NULL;
	strcpy(P->nom,nom);
	P->etat = activable;
	P->registres[esp] = (uint32_t)((P->pile)+STACK_LENGTH-3);
	P->pile[ssize-3] = (uint32_t)processus;
	P->pile[ssize-2] = (uint32_t)exit;
	P->pile[ssize-1] = (uint32_t)arg;
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
	P->prec=processus_actif->child;
	// si la prio du processus cree est superieure a celle du processus actif
	if (prio > processus_actif->prio) {
		ordonnancement();
	}
}

uint32_t start(int(*code)(void*), unsigned long ssize, int prio, const char * nom, void *arg){
	for (int futur_pid=0; futur_pid<NBPROC; futur_pid++){
		if (table_processus[futur_pid] == NULL){
			init(futur_pid,nom,ssize,prio,code, arg);
			return futur_pid;
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

void ordonnancement(void){
	Processus* prochain;
	Processus* top;
	Processus* ancien;

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

			// arret de la boucle si on a trouve un processus a activer
			// en fin de boucle car top peut avoir change d'etat entre temps
			if (top->etat==activable) {
				prochain=top;
				break;
			}
		}

	} else {
		// idle
		prochain = table_processus[0];
	}

	// enfin on evitera qu'un processus ne se passe la main a lui-meme
	// if (prochain != processus_actif) {
		prochain->etat=actif;
		ancien = processus_actif;
		ancien->etat=(ancien->etat==actif)?activable:ancien->etat;
		processus_actif = prochain;
		ctx_sw(ancien->registres, prochain->registres);
	// }

}


void exit(int retval){
	retval = read_eax();
	if (processus_actif->pere != NULL){
		processus_actif->etat = zombie;
		processus_actif->retval = retval;
	} else {
		kill(processus_actif->pid);
	}
	ordonnancement();
	while(1) {}
}


int kill(int pid){
	if ((pid >=0) && (pid<NBPROC) && (table_processus[pid] != NULL)){
		// cas d'un pere : un processus peut appeler kill explicitement
		// donc redondant avec le code de exit
		if (table_processus[pid]->pere != NULL){
			table_processus[pid]->etat = zombie;

		} else {
			kill_childs(table_processus[pid]);
			// on enleve le processus de la file
			queue_del(processus_actif, lien);
			table_processus[processus_actif->pid] = NULL;
			free(processus_actif);
		}
		ordonnancement();
		return 0;
	} else {
		return -1;
	}

}

int waitpid(int pid, int *retvalp){

	if (processus_actif->child == NULL){
		return -1;
	} else {
		Processus * child = processus_actif->child;
		if (pid<0){
			while(child !=NULL){
				if (child->etat == zombie){
					*retvalp = child->retval;
					kill_childs(child);
					queue_del(child, lien);
					table_processus[child->pid] = NULL;
					free(child);
					return pid;
				}
				child = child->suiv;
			}
			processus_actif->etat = wait_child;
			ordonnancement();
		}
		else {
			while(child!=NULL){
				if(child->pid == pid){
					if(child->etat == zombie){
						if (retvalp != NULL) {
							*retvalp = child->retval;
						}
						kill_childs(child);
						queue_del(child, lien);
						table_processus[pid] = NULL;
						free(child);
						// on enleve le processus de la file


						return child->pid;
					}
					processus_actif->etat = wait_child;
					ordonnancement();
				}
			}
			return -1;
		}

	}
	return -1;
}




int getprio(int pid){
	if (pid<0 || pid>=NBPROC || table_processus[pid] == NULL){
		return -1;
	} else {
		return table_processus[pid]->prio;
	}
}


int chprio(int pid, int newprio){
	if (pid<0 || pid>=NBPROC || table_processus[pid] == NULL){
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

/*
Lors de la mort du pere, les fils doivent :
- etre detruits s'ils sont zombies
- etre marques comme n'ayant plus de pere
*/
void kill_childs(Processus *P){
	Processus* child = P->child;
	while (child != NULL){
		// si on tombe sur un zombie, il doit etre detruit car aucun pere pour l'attendre
		if (child->etat == zombie){
			queue_del(child, lien);
			table_processus[child->pid] = NULL;
			free(child);
		// sinon il n'y a plus de pointeur sur le pere
		} else {
			child->pere = NULL;
		}
		child = child->suiv;
	}
	if (P->pere != NULL) {
		child = P->pere->child;
		while (child != NULL && child->pid != P->pid) {
			child=child->suiv;
		}
		if (child->suiv != NULL) {
			child->suiv->prec=child->prec;
			child->prec->suiv=child->suiv;
		} else {
			P->pere->child=NULL;
		}
	}
}

void wait_clock(uint32_t nbr_secs){
	processus_actif->reveil += nbr_secs;
	processus_actif->etat = endormi;
	ordonnancement();
}
