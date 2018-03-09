#include "debugger.h"
#include "cpu.h"
#include <stdio.h>
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"
#include "processus.h"

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}


void kernel_start(void)
{


	efface_ecran();
	printf("Début tests changement de processus !\n");

	//Initialisation de idle
	init_idle();

	//Création des 3 processus
	start(proc1,"proc1",0,3);
	start(proc2,"proc2",0,3);
	start(proc3,"proc3",0,0);
	start(proc4,"proc4",0,2);


	//Lancement de idle (Il va ensuite lancer les autres)
	idle();

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);
	sti();


	affiche_h();
	while(1)
	  hlt();

	return;
}
