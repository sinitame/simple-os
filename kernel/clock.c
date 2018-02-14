#include"../user/lib/clock.h"
#include <stdbool.h>
#include <cpu.h>
#include <inttypes.h>
#include <start.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <segment.h>
#include "../user/lib/console_putbytes.h"

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
  sec = (temps/50)%60;
  min = (temps/(50*60))%60;
  heure = (temps/(50*60*60)) % 24;
  affiche_h();
  //ordonnance();

}

void masque_IRQ(/*uint32_t num_IRQ, bool masque*/)
{
  uint8_t octet = inb(0x21);
  octet = octet & 0xFE;
  outb(octet,0x21);

}




void clk(void)
{
  outb(0x34,0x43);
  outb((QUARTZ/CLOCKFREQ)%256,0x40);
  outb((QUARTZ/CLOCKFREQ)>>8,0x40);
}

void init_traitan_IT(uint32_t num_IT, void (*traitant)(void))
{
  uint32_t *ptr_IT = (uint32_t*)0x1000 + num_IT*2;
  *ptr_IT = (KERNEL_CS<<16 | ((uint32_t)traitant & 0xFFFF)) ;
  *(ptr_IT+1) = (((uint32_t)traitant & 0xFFFF0000)| 0x8E00);
}
