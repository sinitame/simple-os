#include "cpu.h"
#include "test0.h"
#include "stdio.h"
#include "../shared/stddef.h"
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"
#include "processus.h"

 #define DUMMY_VAL 78


int dummy1(void *arg) {
        printf("1");
        assert((int) arg == DUMMY_VAL);
        return 3;
}

void kernel_start(void)
{

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	efface_ecran();
	init_idle();
	start(dummy1, 4000, 192, "proc1", (void*)DUMMY_VAL);


	idle();

	while(1)
	  hlt();

	return;
}
