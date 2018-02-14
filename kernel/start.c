#include "debugger.h"
#include "cpu.h"
#include <stdio.h>
#include "../user/lib/console_putbytes.h"

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}


void kernel_start(void)
{
	int i;
//	call_debugger();

	i = 10;

	i = fact(i);

	efface_ecran();

	printf("Hello world !");
	printf("i= %d", i);

	while(1)
	  hlt();

	return;
}
