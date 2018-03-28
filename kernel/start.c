#include "cpu.h"
#include "test0.h"
#include "test1.h"
#include "stdio.h"
#include "../shared/stddef.h"
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"
#include "processus.h"

void kernel_start(void)
{

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	efface_ecran();
	test1(0);

	while(1)
	  hlt();

	return;
}
