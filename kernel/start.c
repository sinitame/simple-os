#include "debugger.h"
#include "cpu.h"
#include <stdio.h>
#include "../shared/console.h"
#include <inttypes.h>
#include <start.h>
#include "../user/lib/clock.h"
#include "processus.h"
#include "message_queue.h"

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

	//Initialisation de idle
	init_idle();

	int fid;
        int pid;
        int msg;
        int count;
	
        //assert(getprio(getpid()) == 128);
	/*int testpid=getpid();
	printf("%d", testpid);
	int testprio=getprio(testpid);
	printf("%d", testprio);*/
	
        assert((fid = pcreate(1)) >= 0);
        printf("1\n");
        pid = start(rdv_proc, 4000, 130, "rdv_proc", (void *)fid);
        assert(pid > 0);
        printf(" 4\n");
        //assert((pcount(fid, &count) == 0) && (count == 2));
	pcount(fid, &count);
	printf("count = %d\n", count);
        assert(preceive(fid, &msg) == 0); /* Retire du tampon et debloque un emetteur. */
        assert(msg == 3);
        printf(" 7");
        assert((pcount(fid, &count) == 0) && (count == -1));
        assert(psend(fid, 5) == 0); /* Pose dans le tampon. */
        printf(" 9");
        assert(psend(fid, 6) == 0); /* Pose dans le tampon. */
        assert(preceive(fid, &msg) == 0); /* Retire du tampon. */
        assert(msg == 6);
        assert(pdelete(fid) == 0);
        assert(psend(fid, 2) < 0);
        assert(preceive(fid, &msg) < 0);
        assert(waitpid(-1, 0) == pid);
        printf(" 10.\n");


	//Lancement de idle (Il va ensuite lancer les autres)
	idle();

	clk();
	masque_IRQ(0,0);
	init_traitant_IT(32, traitant_IT_32);
	sti();


	affiche_h();
	while(1)
	  hlt();

	return;
}
