#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include <processus.h>
#include <string.h>


extern void ctx_sw(int*,int*);

void init_idle(){

	for (int i = 0; i<NBPROC; i++){
		table_processus[i].pid = -1;
	}

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
		ordonnancement_simple();
	}
}
void proc1(void)
{
	for(;;){
		printf("[%s] pid = %d \n",mon_nom(),mon_pid());
		ordonnancement_simple();
	}
}

void proc2(void){
	for(;;){
		printf("[%s] pid = %d \n",mon_nom(),mon_pid());
		ordonnancement_simple();
	}
}

void proc3(void){
	for(;;){
		printf("[%s] pid = %d \n",mon_nom(),mon_pid());
		ordonnancement_simple();
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
