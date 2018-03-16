#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include <processus.h>
#include <string.h>
#include "../shared/malloc.c"


extern void ctx_sw(int*,int*);

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

	//Initialisation du processus tueur
	table_processus[NBPROC] = malloc(sizeof(Processus));
	Processus *Ptueur = table_processus[NBPROC];
	Ptueur->pid = NBPROC;
	Ptueur->pere = NULL;
	Ptueur->prio = 0;
	Ptueur->childs = NULL;
	strcpy(Ptueur->nom,"tueur");
	Ptueur->etat = activable;
	Ptueur->registres[esp] = (uint32_t)((Ptueur->pile)+511);
	Ptueur->pile[511] = (uint32_t)tueur;
	

}

void init(int pid, char* nom, int etat,int prio, void (*processus)(void)){
	
	//Création du nouveau processus
	table_processus[pid] = malloc(sizeof(Processus));
	Processus *P = table_processus[pid];
	P->pid = pid;
	P->pere = processus_actif;
	P->prio = prio;
	P->childs = NULL;
	strcpy(P->nom,nom);
	P->etat = etat;
	P->registres[esp] = (uint32_t)((P->pile)+511);
	P->pile[511] = (uint32_t)processus;

	//Ajout du processus à la file des processus
	queue_add(P,&file_processus,Processus,lien,prio);

	//Ajout du processus crée à la liste des fils du process actif
	Liste new_child = malloc(sizeof(*new_child));
	new_child->pid = pid;
	new_child->suiv = processus_actif->childs;
	processus_actif->childs = new_child; 
}

uint32_t start(void(*code)(void), char * nom, int taille_pile, int prio){
	for (int futur_pid=0; futur_pid<NBPROC; futur_pid++){
		if (table_processus[futur_pid] == NULL){
			init(futur_pid,nom,activable,prio,code);
			printf("pid cree :%d \n", futur_pid);
			return futur_pid;
		}
	}
	
	
	taille_pile+=1;
	return -1;
}
	


void idle(void)
{
	for (;;){
		printf("[%s] pid = %d \n",mon_nom(),getpid());
		ordonnancement();
	}
}

void tueur(void){
	table_processus[processus_actif->pid] = NULL;
	printf("pid : %d tuer par le tueur \n",processus_actif->pid);
	free(processus_actif);
	ordonnancement();
}
	

void proc1(void)
{
	for(;;){
		printf("[%s] pid = %d , pere : %d\n",mon_nom(),getpid(),processus_actif->pere->pid);
		ordonnancement();
	}
}

void proc2(void){
	for(;;){
		printf("[%s] pid = %d , pere : %d\n",mon_nom(),getpid(),processus_actif->pere->pid);
		start(proc3,"fils2",0,3);
		ordonnancement();
	}
}

void proc3(void){
	for(;;){
		printf("[%s] pid = %d , pere : %d\n",mon_nom(),getpid(),processus_actif->pere->pid);
		kill(1);
		start(proc4,"fils3",0,3);
		ordonnancement();
	}
}

void proc4(void){
	for(;;){
		printf("[%s] pid = %d , pere : %d\n",mon_nom(),getpid(),processus_actif->pere->pid);
		kill(2);
		ordonnancement();
	}
}

void ordonnancement_simple(){
	int pid_actif = processus_actif->pid;
	Processus* prochain;
	Processus* ancien;

	prochain = table_processus[(pid_actif + 1)%NBPROC];
	
	processus_actif->etat = activable;
	prochain->etat=actif;
	ancien = processus_actif;
	processus_actif = prochain;
	
	ctx_sw(ancien->registres, prochain->registres);
	

	
	
}

void ordonnancement(void){
	Processus* prochain;
	Processus* top;
	Processus* ancien;

	int time=1;

	//On verifie bien que si l'actif à été endormi, 
	//On ne le reveil pas et que s'il est actif on le remet dans la file
	if (processus_actif->etat == actif){
		processus_actif->etat = activable;
		queue_add(processus_actif,&file_processus,Processus,lien,prio);
		
	}

	if (processus_actif->etat == mort){
		ctx_sw(processus_actif->registres, table_processus[NBPROC]->registres);
	}
		
	
	//Verification qu'il y a bien des process à executer, sinon c'est idle qui est choisit.
	if (!queue_empty(&file_processus)){
		top = queue_top(&file_processus,Processus,lien);
		//Choix d'un processus activable
		while (top->etat != activable){
			//Actualisation des etats endormi si necessaire
			if (top->etat == endormi && time>top->reveil){
				top->etat = activable;
			} 
			top = queue_top(&top->lien,Processus,lien);
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


void exit1(int retval){
	if (processus_actif->pere != NULL){
		processus_actif->etat = zombie;
		processus_actif->retval = retval;
	} else {
		processus_actif->etat = mort;
		kill_childs(processus_actif);
	}
	ordonnancement();
}


int kill(int pid){
	if ((pid >=0) && (pid<NBPROC) && (table_processus[pid] != NULL)){
		if (processus_actif->pere != NULL){
			table_processus[pid]->etat = zombie;
			printf("pid : %d deviens zombie \n",pid);
		} else {
			table_processus[pid]->etat = mort;
			kill_childs(table_processus[pid]);
			printf("pid : %d mort \n",pid);
		}
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
		} else {
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

void kill_childs(Processus *P){
	Liste childs = P->childs;
	Processus * child;
	while (childs != NULL){
		child = table_processus[childs->pid];
		if (child != NULL){
			if (child->etat == zombie){
				free(table_processus[child->pid]);
				table_processus[child->pid] = NULL;
				printf("pid : %d tuer par son pere %d \n",child->pid,P->pid);
			} else {
				child->pere = NULL;
			}
		}
		childs = childs->suiv;
	}
}

void dors(uint32_t nbr_secs){
	processus_actif->reveil = nbr_secs;
	processus_actif->etat = endormi;
	ordonnancement();
}