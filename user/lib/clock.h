#include "../kernel/cpu.h"
#include "../shared/inttypes.h"
#include <stdbool.h>


#ifndef __CLOCK_H__
#define __CLOCK_H__



void affiche_h(void);
void tic_PIT(void);
void masque_IRQ(/*uint32_t num_IRQ, bool masque*/);
void clk(void);
void init_traitan_IT(uint32_t num_IT, void (*traitant)(void));
extern uint32_t temps;
extern uint32_t sec;

#endif
