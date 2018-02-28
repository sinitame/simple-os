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

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);
	sti();


	affiche_h();
	while(1)
	  hlt();

	return;
}
