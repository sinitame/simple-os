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
	strcpy(P->nom,"idle");
	P->etat = actif;
	processus_actif = P;

}

void init(int pid, const char* nom, unsigned long ssize,int prio, int (*processus)(void*), void *arg){

	//Création du nouveau processus
	table_processus[pid] = malloc(sizeof(Processus));
	Processus *P = table_processus[pid];
	P->pid = pid;
	P->pere = processus_actif;
	P->prio = prio;
	P->childs = NULL;
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
	Liste new_child = malloc(sizeof(*new_child));
	new_child->pid = pid;
	new_child->suiv = processus_actif->childs;
	processus_actif->childs = new_child;
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

	if (processus_actif->etat == mort){
		table_processus[processus_actif->pid] = NULL;
		free(processus_actif);
	}

	//On verifie bien que si l'actif à été endormi,
	//On ne le reveil pas et que s'il est actif on le remet dans la file
	if (processus_actif->etat == actif){
		processus_actif->etat = activable;
		queue_add(processus_actif,&file_processus,Processus,lien,prio);
	}

	if (processus_actif->etat == endormi){
		queue_add(processus_actif,&file_processus,Processus,lien,prio);
	}


	if(processus_actif->etat == zombie && processus_actif->pere-> etat == wait_child){
		processus_actif->pere->etat = activable;
		queue_add(processus_actif->pere,&file_processus,Processus,lien,prio);
	}





	//Verification qu'il y a bien des process à executer, sinon c'est idle qui est choisit.
	if (!queue_empty(&file_processus)){
		top = queue_top(&file_processus,Processus,lien);
		//Choix d'un processus activable
		while (top->etat != activable){
			//Actualisation des etats endormi si necessaire
			if (top->etat == endormi && sec>top->reveil){
				top->etat = activable;
			}
			// fix temporaire pour test2
			if (top->pere != NULL) {
				top = top->pere;
			}
		}

		//Suppression du processus choisi
		prochain = top;
		queue_del(top,lien);


	} else {
		prochain = table_processus[0];
	}



	//Contexte switch
	prochain->etat=actif;
	ancien = processus_actif;
	processus_actif = prochain;
	ctx_sw(ancien->registres, prochain->registres);


}


void exit(int retval){
	retval = read_eax();
	if (processus_actif->pere != NULL){
		processus_actif->etat = zombie;
		processus_actif->retval = retval;
	} else {
		processus_actif->etat = mort;
		kill_childs(processus_actif);
	}
	ordonnancement();
	while(1) {}
}


int kill(int pid){
	if ((pid >=0) && (pid<NBPROC) && (table_processus[pid] != NULL)){
		if (table_processus[pid]->pere != NULL){
			table_processus[pid]->etat = zombie;
		} else {
			table_processus[pid]->etat = mort;
			kill_childs(table_processus[pid]);
		}
		ordonnancement();
		return 0;
	} else {
		return -1;
	}

}

int waitpid(int pid, int *retvalp){

	if (processus_actif->childs == NULL){
		return -1;
	} else {
		if (pid<0){
			Liste childs = processus_actif->childs;
			while(1){
				while(childs !=NULL){
					if (table_processus[childs->pid]->etat == zombie){
						*retvalp = table_processus[childs->pid]->retval;
						kill_childs(table_processus[childs->pid]);
						free(table_processus[childs->pid]);
						table_processus[childs->pid] = NULL;
						return childs->pid;
					}
					childs = childs->suiv;
				}
				childs = processus_actif->childs;
			}
		}
		else {
			Liste childs = processus_actif->childs;
			while(childs!=NULL){
				if(childs->pid == pid){
					while(1){
						if(table_processus[childs->pid]->etat == zombie){
							*retvalp = table_processus[childs->pid]->retval;
							kill_childs(table_processus[childs->pid]);
							free(table_processus[childs->pid]);
							table_processus[childs->pid] = NULL;

							return childs->pid;
						}
						processus_actif->etat = wait_child;
						ordonnancement();
					}
				}
				childs = childs->suiv;
			}
			return -1;
		}

	}
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
		int ancienne_prio = table_processus[pid]->prio;
		table_processus[pid]->prio = newprio;
		//TODO : Supprimer le processus des files d'attente et le remettre avec sa bonne priorité
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
	Liste childs = P->childs;
	Processus * child;
	while (childs != NULL){
		child = table_processus[childs->pid];
		if (child != NULL){ // ne sert a rien mais protection
			// si on tombe sur un zombie, il doit etre detruit
			if (child->etat == zombie){
				free(table_processus[child->pid]);
				table_processus[child->pid] = NULL;
			// sinon il n'y a plus de pointeur sur le pere
			} else {
				child->pere = NULL;
			}
		}
		childs = childs->suiv;
	}
}

void wait_clock(uint32_t nbr_secs){
	processus_actif->reveil += nbr_secs;
	processus_actif->etat = endormi;
	ordonnancement();
}
