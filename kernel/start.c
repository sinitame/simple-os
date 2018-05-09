#include <inttypes.h>
#include <start.h>

#include "cpu.h"
#include "stdio.h"

#include "../shared/stddef.h"
#include "../shared/console.h"
#include "../user/lib/clock.h"
#include "processus.h"

#include "tests/tests.h"

void kernel_start(void)
{

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	efface_ecran();

	init_idle();

	start(test0, 4000, 128, "test0", (void *) 77);
	start(test1, 4000, 128, "test1", (void *) 77);
	start(test2, 4000, 128, "test2", (void *) 77);
	start(test3, 4000, 128, "test3", (void *) 77);
	start(test4, 4000, 128, "test4", (void *) 77);
	start(test5, 4000, 128, "test5", (void *) 77);
	start(test6, 4000, 128, "test6", (void *) 77);
	// start(test7, 4000, 128, "test7", (void* ) 77);
	start(test8, 4000, 128, "test8", (void *) 77);
	// start(test9, 4000, 128, "test9", (void*) 77);
	start(test12, 4000, 128, "test12", (void *) 77);
	// start(test13, 4000, 128, "test13", (void*) 77);
	start(test14, 4000, 128, "test14", (void *) 77);
	start(test15, 4000, 128, "test15", (void *) 77);
	// start(test16, 4000, 128, "test16", (void*)77);
	// start(test17, 4000, 128, "test17", (void*)77);

	idle();

	while(1)
	  hlt();

	return;
}
