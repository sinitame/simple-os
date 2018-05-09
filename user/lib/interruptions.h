#include <inttypes.h>
#include <segment.h>
#include <cpu.h>

#ifndef __INTERRUPTIONS_H__
#define __INTERRUPTIONS_H__

void masque_IRQ(uint32_t num_IRQ, uint8_t masque);
void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));

#endif
