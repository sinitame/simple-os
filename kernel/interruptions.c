#include "../user/lib/interruptions.h"
#include <stdio.h>




void init_traitant_IT(uint32_t num_IT, void (*traitant)(void))
{
  uint32_t *ptr_IT = (uint32_t*)0x1000 + num_IT*2;
  *ptr_IT = (KERNEL_CS<<16 | ((uint32_t)traitant & 0xFFFF)) ;
  *(ptr_IT+1) = (((uint32_t)traitant & 0xFFFF0000)| 0x8E00);
}

void masque_IRQ(uint32_t num_IRQ, uint8_t masque){
	uint8_t masqueAct;
	uint8_t mask;
	mask=1;
	mask=mask<<num_IRQ;
	masqueAct=inb(0x21);
	if (masque==0){
		mask=~mask;
		masqueAct=masqueAct & mask;
	}
	else{
		masqueAct=masqueAct | mask;
	}
	outb(masqueAct,0x21);
}
