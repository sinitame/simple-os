#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include <processus.h>
#include <string.h>


extern void ctx_sw(int*,int*);

void init_idle(){
	
	// Initialisation de la table et de la file des processus
	for (int i = 0; i<NBPROC; i++){
		table_processus[i].pid = -1;
	}

	INIT_LIST_HEAD(&file_processus);
	Processus *P = &table_processus[0];
	P->pid = 0;
	strcpy(P->nom,"idle");
	P->etat = actif;
	processus_actif = P;
}

void init(int pid, char* nom, int etat,int prio, void (*processus)(void)){
	Processus *P = &table_processus[pid];
	P->pid = pid;
	P->prio = prio;
	strcpy(P->nom,nom);
	P->etat = etat;
	P->registres[esp] = (uint32_t)((P->pile)+511);
	P->pile[511] = (uint32_t)processus;
	queue_add(P,&file_processus,Processus,lien,prio);
}

uint32_t start(void(*code)(void), char * nom, int taille_pile, int prio){
	for (int futur_pid=0; futur_pid<NBPROC; futur_pid++){
		if (table_processus[futur_pid].pid == -1){
			init(futur_pid,nom,activable,prio,code);
			return futur_pid;
		}
	}
	taille_pile+=1;
	return -1;
}
	


void idle(void)
{
	for (;;){
		printf("[%s] pid = %d \n",mon_nom(),mon_pid());
		ordonnancement();
	}
}
void proc1(void)
{
	for(;;){
		printf("[%s] pid = %d \n",mon_nom(),mon_pid());
		ordonnancement();
	}
}

void proc2(void){
	for(;;){
		printf("[%s] pid = %d \n",mon_nom(),mon_pid());
		ordonnancement();
	}
}

void proc3(void){
	for(;;){
		printf("[%s] pid = %d \n",mon_nom(),mon_pid());
		ordonnancement();
	}
}


void ordonnancement_simple(){
	int pid_actif = (*processus_actif).pid;
	Processus* prochain;
	Processus* ancien;

	prochain = &table_processus[(pid_actif + 1)%NBPROC];
	
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
		prochain = &table_processus[0];
	}

	//On verifie bien que si l'actif à été endormi, on ne le reveil pas et que s'il est actif on le remet dans la file
	if (processus_actif->etat == actif){
		processus_actif->etat = activable;
		queue_add(processus_actif,&file_processus,Processus,lien,prio);
		
	}

	//Contexte switch
	prochain->etat=actif;
	ancien = processus_actif;
	processus_actif = prochain;
	ctx_sw(ancien->registres, prochain->registres);
		

}

int mon_pid(void){
	return processus_actif->pid;
}

char* mon_nom(void){
	return processus_actif->nom;
}

void dors(uint32_t nbr_secs){
	processus_actif->reveil = nbr_secs;
	processus_actif->etat = endormi;
	ordonnancement();
}
