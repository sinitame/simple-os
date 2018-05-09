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

#include "tests/tests.h"
#include "userspace_apps.h"
#include "hash.h"

#include "shell.h"


void kernel_start(void)
{
  // efface_ecran();
  // hash_t *map=create_hash();
  // if (hash_isset(map, "psender")!=0) {
  //     printf("app trouvé");
  //   }

  clk();
	masque_IRQ(0,0);
	masque_IRQ(1,0);
	init_traitant_IT(32, traitant_IT_32);
	init_traitant_IT(33, traitant_IT_33);
	init_buff(&stdin,5);

	efface_ecran();

	init_idle();

    /* processus */
	// start(test0, 4000, 128, "test0", (void *) 77);
	// start(test1, 4000, 128, "test1", (void *) 77);
	// start(test2, 4000, 128, "test2", (void *) 77);
	// start(test3, 4000, 128, "test3", (void *) 77);
	start(test4, 4000, 128, "test4", (void *) 77);
	// start(test5, 4000, 128, "test5", (void *) 77);
	// start(test6, 4000, 128, "test6", (void *) 77);
	//
    // start(killer, 4000, 128, "killer", (void *)77);

    // // tres long
	// start(test7, 4000, 128, "test7", (void* ) 77);

	// start(test8, 4000, 128, "test8", (void *) 77);

    // // tres long
	// start(test9, 4000, 128, "test9", (void*) 77);

    /* file de message */

	// start(test12, 4000, 128, "test12", (void *) 77);

    // // -- ne passe pas
	// start(test13, 4000, 128, "test13", (void*) 77);
    // start(test16, 4000, 128, "test16", (void*)77);

	// start(test14, 4000, 128, "test14", (void *) 77);
	// start(test15, 4000, 128, "test15", (void *) 77);
	// start(test17, 4000, 128, "test17", (void*)77);

    /* console */

    // start(test_cons_read, 4000, 128, "test_cons_read", (void *)77);

    // // passe pas
	// start(test18, 4000, 128, "test18", (void *)77);
	// start(test19, 4000, 128, "test19", (void *)77);
	// start(test20, 4000, 128, "test20", (void *)77);
	// start(test21, 4000, 128, "test21", (void *)77);
	// start(test22, 4000, 128, "test22", (void *)77);

    /* shell */
    // sti();
    // start(shell, 4000, 128, "shell", (void *) 77);

	idle();

	while(1)
	  hlt();

	return;
}
