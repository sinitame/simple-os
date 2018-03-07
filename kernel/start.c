#include "cpu.h"
#include "test0.h"
#include "stdio.h"
#include "console.h"

void kernel_start(void)
{
	efface_ecran();
	test0(0);

	while(1)
	  hlt();

	return;
}
