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

	//test1(0);
	start(test2, 4000, 128, "test2", (void *) 77);


	while(1)
	  hlt();

	return;
}
