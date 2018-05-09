#include"../user/lib/clock.h"
#include <stdbool.h>
#include <cpu.h>
#include <inttypes.h>
#include <start.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <segment.h>
#include "../kernel/processus.h"
#include "../user/lib/console_putbytes.h"
#include "processus.h"

#define QUARTZ 0x1234DD
#define CLOCKFREQ 50

uint32_t sec=0;
uint32_t min=0;
uint32_t heure=0;
uint32_t temps = 0;



void affiche_h(void)
{
  char c[20];
  sprintf(c,"uptime:%02u:%02u:%02u",heure,min,sec);
  int i;
  for(i=0;i<15;i++)
  {
    ecrit_car(0,65+i,c[i]);
  }
}

void tic_PIT(void)
{
  outb(0x20,0x20);
  inb(0x21);
  temps++;
  if (temps%CLOCKFREQ == 0){
      sec++;
      min=(sec==60)?(min+1):min;
      heure=(min==60)?(heure+1):heure;
      min=(min==60)?(0):min;
      sec=(sec==60)?(0):sec;
  }
//  printf("temps: %d", temps);
  affiche_h();
  ordonnancement();
}



void clk(void)
{
  outb(0x34,0x43);
  outb((QUARTZ/CLOCKFREQ)%256,0x40);
  outb((QUARTZ/CLOCKFREQ)>>8,0x40);
}




void clock_settings(unsigned long *quartz, unsigned long *ticks){
  *quartz = QUARTZ;
  *ticks = CLOCKFREQ;
}

uint32_t current_clock(){
  return temps;
}
