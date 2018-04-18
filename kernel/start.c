#include "debugger.h"
#include "cpu.h"
#include <stdio.h>
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"
#include "processus.h"
// #include "test12.h"
// #include "test14.h"
#include "test15.h"

void kernel_start(void)
{


	efface_ecran();

		clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	init_idle();

	//test12(0);
	start(test15, 4000, 128, "test15", (void *) 77);


	while(1)
	  hlt();

	return;
}
