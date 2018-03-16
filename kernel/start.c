#include "cpu.h"
#include "test0.h"
#include "test1.h"
#include "stdio.h"
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"
#include "processus.h"

// static void dummy2(void)
// {
//         printf(" 5");
//         // assert((int) arg == DUMMY_VAL + 1);
//         // return 4;
// }
//
// static void dummy1(void) {
//         printf("1");
//         // assert((int) arg == DUMMY_VAL);
//         // return 3;
// }

void kernel_start(void)
{

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	efface_ecran();
	//Initialisation de idle
	//Lancement de idle (Il va ensuite lancer les autres)
	init_idle();
	start(proc1, "dummy1", 4000, 192);
	start(proc2, "dummy2", 4000, 100);

	sti();
	idle();
	while(1)
	  hlt();

	return;
}
