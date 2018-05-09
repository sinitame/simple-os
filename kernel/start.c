#include <inttypes.h>
#include <start.h>

#include "cpu.h"
#include "stdio.h"

#include "../shared/stddef.h"
#include "../shared/console.h"
#include "../user/lib/clock.h"
#include "processus.h"

#include "tests.h"
#include "userspace_apps.h"
#include "hash.h"

void kernel_start(void)
{
  // efface_ecran();
  // hash_t *map=create_hash();
  // if (hash_isset(map, "psender")!=0) {
  //     printf("app trouvé");
  //   }

  clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	efface_ecran();


	init_idle();


	start(test4, 4000, 128, "test4", (void *) 77);
	//start(test5, 4000, 128, "test5", (void *) 77);


	idle();

	while(1)
	  hlt();

	return;
}
