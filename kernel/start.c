#include <inttypes.h>
#include <start.h>

#include "cpu.h"
#include "stdio.h"

#include "../shared/stddef.h"
#include "../shared/console.h"
#include "../user/lib/clock.h"
#include "processus.h"

#include "tests.h"

void kernel_start(void)
{

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	efface_ecran();
	init_idle();

	// start(test1, STACK_LENGTH, 128, "test1", (void *) 77);
	// start(test2, STACK_LENGTH, 128, "test2", (void *) 77);
	start(test3, STACK_LENGTH, 128, "test3", (void *) 77);
	// start(test4, STACK_LENGTH, 128, "test4", (void *) 77);
	// start(killer, STACK_LENGTH, 128, "killer", (void *) 0);

	idle();

	while(1)
	  hlt();

	return;
}
