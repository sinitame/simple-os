#include <inttypes.h>


#ifndef __CLOCK_H__
#define __CLOCK_H__



void affiche_h(void);
void tic_PIT(void);
void masque_IRQ(uint32_t num_IRQ, uint8_t masque);
void clk(void);
void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));
extern uint32_t temps;
extern uint32_t sec;

void clock_settings(unsigned long *quartz, unsigned long *ticks);
uint32_t current_clock();

#endif
