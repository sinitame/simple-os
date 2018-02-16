#include "debugger.h"
#include "cpu.h"
#include <stdio.h>
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}


void kernel_start(void)
{
	int i;
//	call_debugger();

	i = 10;

	i = fact(i);

	efface_ecran();

	printf("Hello world !");
	printf("i= %d", i);
	
	masque_IRQ(/*uint32_t num_IRQ, bool masque*/);
	init_traitan_IT(32, traitant_IT_32);
	sti();


	affiche_h();
	while(1)
	  hlt();

	return;
}
