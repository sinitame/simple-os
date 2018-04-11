#include "debugger.h"
#include "cpu.h"
#include <stdio.h>
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"
#include "processus.h"
#include "message_queue.h"
#include "test12.h"

int rdv_proc(void *arg) {
  int fid = (int) arg;
  int msg;
  int count;
  printf(" 2");
  assert(psend(fid, 3) == 0); /* Depose dans le tampon */
  printf(" 3");
  assert((pcount(fid, &count) == 0) && (count == 1));
  assert(psend(fid, 4) == 0); /* Bloque tampon plein */
  printf(" 5");
  assert((pcount(fid, &count) == 0) && (count == 1));
  assert(preceive(fid, &msg) == 0); /* Retire du tampon */
  assert(msg == 4);
  printf(" 6");
  assert(preceive(fid, &msg) == 0); /* Bloque tampon vide. */
  assert(msg == 5);
  printf(" 8");
  assert((pcount(fid, &count) == 0) && (count == 0));
  return 0;
}
void kernel_start(void)
{


	efface_ecran();
	printf("Début tests file de messages !\n");

		clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);

	init_idle();

	//test12(0);
	start(test12, 4000, 128, "test1", (void *) 77);


	while(1)
	  hlt();

	return;
}
