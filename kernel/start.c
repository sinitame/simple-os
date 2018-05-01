#include <inttypes.h>
#include <start.h>

#include "cpu.h"
#include "stdio.h"
#include "kbd.h"

#include "../shared/stddef.h"
#include "../shared/console.h"
#include "../user/lib/clock.h"
#include "../user/lib/interruptions.h"
#include "processus.h"

#include "shell.h"

void kernel_start(void)
{

	clk();
	masque_IRQ(0,0);
	masque_IRQ(1,0);
	init_traitant_IT(32, traitant_IT_32);
	init_traitant_IT(33, traitant_IT_33);
	init_buff(&stdin,5);

	efface_ecran();

	init_idle();
	sti();


	// start(test1, 4000, 128, "test1", (void *) 77);
	// start(test2, 4000, 128, "test2", (void *) 77);
	// start(test3, 4000, 128, "test3", (void *) 77);
	// start(test4, 4000, 128, "test4", (void *) 77);
	// start(test5, 4000, 128, "test5", (void *) 77);
	//start(test6, 4000, 128, "test6", (void *) 77);
	start(shell, 4000, 128, "shell", (void *) 77);

	// start(killer, 4000, 128, "killer", (void *) 0);




	idle();




	while(1)
	  hlt();

	return;
}
